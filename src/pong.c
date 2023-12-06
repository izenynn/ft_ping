#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h>

#include "libft/ft_mem.h"

#include "ft_ping.h"

static int pkt_size = 0;

static void print_verbose(void) {
	const char *pkt = (char *)progconf.pkt;
	const struct iphdr *iphdr = (const struct iphdr *)pkt;
	const struct icmphdr *icmphdr = (const struct icmphdr *)(pkt + (iphdr->ihl << 2));

	char saddr[INET_ADDRSTRLEN];
	char daddr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(iphdr->saddr), saddr, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &(iphdr->daddr), daddr, INET_ADDRSTRLEN);

	printf("IP Hdr Dump:\n");
	for (size_t i = 0; i < (size_t)(iphdr->ihl << 2); ++i) {
		if (i % 2 == 0) printf(" ");
		printf("%02x", ((char *)iphdr)[i] & 0xFF);
	}
	printf("\n");

	printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data\n");
	printf(" %1x %2x  %02x %04x %04x   %1x %04x  %02x  %02x %04x %-10s %-10s\n",
		iphdr->version,
		iphdr->ihl,
		iphdr->tos,
		ntohs(iphdr->tot_len),
		ntohs(iphdr->id),
		(ntohs(iphdr->frag_off) >> 13) & 0x07,
		ntohs(iphdr->frag_off) & 0x1FFF,
		iphdr->ttl,
		iphdr->protocol,
		ntohs(iphdr->check),
		saddr,
		daddr);

	printf("ICMP: type %d, code %d, size %ld, id 0x%04x, seq 0x%04x\n",
		icmphdr->type,
		icmphdr->code,
		sizeof(struct iphdr) + sizeof(struct icmphdr) + progconf.args.size - (iphdr->ihl << 2),
		ntohs(icmphdr->un.echo.id),
		ntohs(icmphdr->un.echo.sequence));
}

static void print_info(void)
{
	printf("%d bytes from ", pkt_size);
	if (progconf.args.numeric) {
		printf("%s: ", progconf.host);
	} else {
		printf("%s (%s): ", progconf.rhost, progconf.host);
	}
}

static void parse_pkt(char *buffer, ssize_t size,
		      struct iphdr **iphdr, struct icmphdr **icmphdr)
{
	int iphdr_len;

	if (size < (int)(sizeof(struct iphdr) + sizeof(struct icmphdr))) {
		log_exit(EXIT_FAILURE,
			  "incomplete echo reply package (size: '%z') icmp packet min size: %z",
			  size, sizeof(struct iphdr) + sizeof(struct icmphdr));
	}

	*iphdr = (struct iphdr *)buffer;
	// iphdr->ihl is in 32-bit words, so we shift to get the length in bytes
	iphdr_len = (*iphdr)->ihl << 2;
	*icmphdr = (struct icmphdr *)(buffer + iphdr_len);

	pkt_size = ntohs((*iphdr)->tot_len) - iphdr_len;
}

static void handle_other(struct icmphdr *icmphdr)
{
	print_info();
	switch (icmphdr->type) {
	case ICMP_DEST_UNREACH:
		printf("Destination host unreachable\n");
		break;
	case ICMP_SOURCE_QUENCH:
		printf("Source quench\n");
		break;
	case ICMP_REDIRECT:
		printf("Redirect message\n");
		break;
	case ICMP_TIME_EXCEEDED:
		printf("Time to live exceeded\n");
		break;
	case ICMP_PARAMETERPROB:
		printf("Parameter problem\n");
		break;
	default:
		printf("Unknown ICMP type\n");
	}

	if (progconf.args.verbose)
		print_verbose();
}

static void handle_echoreply(struct ping_stat *const stat,
			     struct iphdr *iphdr, struct icmphdr *icmphdr,
			     double time)
{
	if (time < stat->tmin)
		stat->tmin = time;
	if (time > stat->tmax)
		stat->tmax = time;
	stat->tsum += time;
	stat->tsumsq += time * time;

	++progconf.ping_num_recv;

	if (progconf.args.flood)
		return;

	if (progconf.ping_num_xmit - 1 == ntohs(icmphdr->un.echo.sequence)) {
		print_info();
		printf("icmp_seq=%d ttl=%d time=%.3lf ms\n",
		       ntohs(icmphdr->un.echo.sequence),
		       iphdr->ttl,
		       time);
	} else if (progconf.args.verbose) {
		print_info();
		printf("received icmp_seq=%d later\n",
		       ntohs(icmphdr->un.echo.sequence));
	}
}

static ssize_t recv_pkt(int sockfd, char *buffer, size_t buffer_size,
			enum pong_status *status)
{
	ssize_t size;
	struct iovec io = {
		.iov_base = buffer,
		.iov_len = buffer_size
	};
	struct msghdr msg = {
		.msg_name = NULL,
		.msg_namelen = 0,
		.msg_iov = &io,
		.msg_iovlen = 1,
		.msg_control = NULL,
		.msg_controllen = 0,
		.msg_flags = 0
	};

	size = recvmsg(sockfd, &msg, 0);
	if (size <= 0) {
		if (errno == EINTR || errno == EAGAIN) {
			if (progconf.args.flood) {
				printf(".");
				// fflush(stdout);
			} else {
				log_verbose("request timed out for icmp_seq=%d",
					    progconf.ping_num_xmit - 1);
			}
			*status = PONG_TIMEOUT;
		} else {
			log_pexit(EXIT_FAILURE, "recvmsg");
		}
	} else {
		*status = PONG_SUCCESS;
	}

	return size;
}

enum pong_status pong(const int sockfd, struct ping_stat *const stat)
{
	ssize_t size;
	char buffer[1024];
	struct timeval start, end;
	struct iphdr *iphdr;
	struct icmphdr *icmphdr;

	ft_bzero(buffer, sizeof(buffer));
	gettimeofday(&start, NULL);
	
	enum pong_status status;
	// size <= 0 handled inside
	size = recv_pkt(sockfd, buffer, sizeof(buffer), &status);
	if (status != PONG_SUCCESS)
		return status;
	
	gettimeofday(&end, NULL);
	double time = (double)(end.tv_sec - start.tv_sec) * 1000.0
		      + (double)(end.tv_usec - start.tv_usec) / 1000.0;
	
	parse_pkt(buffer, size, &iphdr, &icmphdr);

	if (icmphdr->type == ICMP_ECHO)
		return PONG_RETRY;
	else if (icmphdr->type == ICMP_ECHOREPLY)
		handle_echoreply(stat, iphdr, icmphdr, time);
	else
		handle_other(icmphdr);

	return PONG_SUCCESS;
}
