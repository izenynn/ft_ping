#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "libft/ft_fd.h"

void receive_pong(int sockfd)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	
	struct timeval start, end;
	gettimeofday(&start, NULL);
	
	if (recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL) <= 0) {
		perror("recvfrom() error");
		exit(1);
	}
	
	gettimeofday(&end, NULL);
	long mtime = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
	
	struct iphdr *ip_packet = (struct iphdr *)buffer;
	// iphdr->ihl is in 32-bit words, so we shift to get the length in bytes
	int ip_header_len = ip_packet->ihl << 2;
	struct icmphdr *icmp_packet = (struct icmphdr *)(buffer + ip_header_len);

	// printf("type offset: %zu\n", (size_t)&(((struct icmphdr *)0)->type));
	// ft_putmem_fd(icmp_packet, sizeof(struct icmphdr), 1);
	// printf("type: %d\n", icmp_packet->type);
	if (icmp_packet->type != ICMP_ECHOREPLY) {
		fprintf(stderr, "Received packet is not an ICMP echo reply\n");
		exit(1);
	}
	
	printf("Received packet from server, RTT = %ld ms\n", mtime);
}
