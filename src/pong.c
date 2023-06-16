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

void pong(const int sockfd, struct ping_stat *const stat)
{
	ssize_t size;
	char buffer[1024];
	struct timeval start, end;

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
	double mtime = (double)(end.tv_sec - start.tv_sec) * 1000.0 + (double)(end.tv_usec - start.tv_usec) / 1000.0;
	
	if (size < (int)(sizeof(struct iphdr) + sizeof(struct icmphdr))) {
		log_exit(EXIT_FAILURE,
			  "incomplete echo reply package (size: '%z') icmp packet min size: %z",
			  size, sizeof(struct iphdr) + sizeof(struct icmphdr));
	}

	struct iphdr *ip_packet = (struct iphdr *)buffer;
	// iphdr->ihl is in 32-bit words, so we shift to get the length in bytes
	int ip_header_len = ip_packet->ihl << 2;
	struct icmphdr *icmp_packet = (struct icmphdr *)(buffer + ip_header_len);

	if (icmp_packet->type != ICMP_ECHOREPLY) {
		printf("%d bytes from %s: ",
			 ntohs(ip_packet->tot_len) - ip_header_len,
			 progconf.host);
		switch (icmp_packet->type) {
		case ICMP_DEST_UNREACH:
			printf("Destination Host Unreachable\n");
			break;
		case ICMP_SOURCE_QUENCH:
			printf("Source Quench\n");
			break;
		case ICMP_REDIRECT:
			printf("Redirect Message\n");
			break;
		case ICMP_TIME_EXCEEDED:
			printf("Time Exceeded\n");
			break;
		case ICMP_PARAMETERPROB:
			printf("Parameter Problem\n");
			break;
		default:
			printf("Unknown ICMP type\n");
		}
		return;
	}

	if (mtime < stat->tmin)
		stat->tmin = mtime;
	if (mtime > stat->tmax)
		stat->tmax = mtime;
	stat->tsum += mtime;
	stat->tsumsq += mtime * mtime;

	if (progconf.ping_num_xmit - 1 == ntohs(icmp_packet->un.echo.sequence)) {
		printf("%d bytes from %s: ",
		       ntohs(ip_packet->tot_len) - ip_header_len,
		       progconf.host);
		printf("icmp_seq=%d ttl=%d time=%.3lf ms",
		       ntohs(icmp_packet->un.echo.sequence),
		       ip_packet->ttl,
		       mtime);
	} else if (progconf.args.verbose) {
		printf("%d bytes from %s: ",
		       ntohs(ip_packet->tot_len) - ip_header_len,
		       progconf.host);
		printf("received icmp_seq=%d later\n",
		       ntohs(icmp_packet->un.echo.sequence));
	}

	++progconf.ping_num_recv;
}
