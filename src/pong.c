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

#include "ft_ping.h"

static int pkt_size = 0;

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
	if (size < (int)(sizeof(struct iphdr) + sizeof(struct icmphdr))) {
		log_exit(EXIT_FAILURE,
			  "incomplete echo reply package (size: '%z') icmp packet min size: %z",
			  size, sizeof(struct iphdr) + sizeof(struct icmphdr));
	}

	*iphdr = (struct iphdr *)buffer;
	// iphdr->ihl is in 32-bit words, so we shift to get the length in bytes
	int iphdr_len = (*iphdr)->ihl << 2;
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

	++progconf.ping_num_recv;
}

void pong(const int sockfd, struct ping_stat *const stat)
{
	ssize_t size;
	char buffer[1024];
	struct timeval start, end;
	struct iphdr *iphdr;
	struct icmphdr *icmphdr;

	memset(buffer, 0, sizeof(buffer));
	gettimeofday(&start, NULL);
	
	size = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
	if (size <= 0) {
		if (errno == EINTR || errno == EAGAIN) {
			log_verbose("request timed out for icmp_seq=%d",
				    progconf.ping_num_xmit - 1);
			return;
		} else {
			log_pexit(EXIT_FAILURE, "recvfrom");
		}
	}
	
	gettimeofday(&end, NULL);
	double time = (double)(end.tv_sec - start.tv_sec) * 1000.0
		      + (double)(end.tv_usec - start.tv_usec) / 1000.0;
	
	parse_pkt(buffer, size, &iphdr, &icmphdr);	
	if (icmphdr->type == ICMP_ECHOREPLY)
		handle_echoreply(stat, iphdr, icmphdr, time);
	else
		handle_other(icmphdr);
}
