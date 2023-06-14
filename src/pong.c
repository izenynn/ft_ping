#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

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
		perror("recvfrom() error");
		// exit(1);
		return;
	}
	// FIXME should this be here or below the ICMP_ECHOREPLY check ?
	++progconf.ping_num_recv;
	
	gettimeofday(&end, NULL);
	double mtime = (double)(end.tv_sec - start.tv_sec) * 1000.0 + (double)(end.tv_usec - start.tv_usec) / 1000.0;
	
	struct iphdr *ip_packet = (struct iphdr *)buffer;
	// iphdr->ihl is in 32-bit words, so we shift to get the length in bytes
	int ip_header_len = ip_packet->ihl << 2;
	struct icmphdr *icmp_packet = (struct icmphdr *)(buffer + ip_header_len);

	if (icmp_packet->type != ICMP_ECHOREPLY) {
		// fprintf(stderr, "Received packet is not an ICMP echo reply\n");
		// exit(1);
		return;
	}

	if (mtime < stat->tmin)
		stat->tmin = mtime;
	if (mtime > stat->tmax)
		stat->tmax = mtime;
	stat->tsum += mtime;
	stat->tsumsq += mtime * mtime;

	printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3lf ms\n",
		ntohs(ip_packet->tot_len) - ip_header_len,
		progconf.host,
		ntohs(icmp_packet->un.echo.sequence),
		ip_packet->ttl,
		mtime);
}
