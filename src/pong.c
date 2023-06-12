#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

void pong(int sockfd, struct addrinfo *addr)
{
	static char ip[INET_ADDRSTRLEN] = {0};

	ssize_t size;
	char buffer[1024];
	struct timeval start, end;

	memset(buffer, 0, sizeof(buffer));
	gettimeofday(&start, NULL);
	
	size = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
	if (size <= 0) {
		perror("recvfrom() error");
		exit(1);
	}
	
	gettimeofday(&end, NULL);
	double mtime = (double)(end.tv_sec - start.tv_sec) * 1000.0 + (double)(end.tv_usec - start.tv_usec) / 1000.0;
	
	struct iphdr *ip_packet = (struct iphdr *)buffer;
	// iphdr->ihl is in 32-bit words, so we shift to get the length in bytes
	int ip_header_len = ip_packet->ihl << 2;
	struct icmphdr *icmp_packet = (struct icmphdr *)(buffer + ip_header_len);

	if (icmp_packet->type != ICMP_ECHOREPLY) {
		fprintf(stderr, "Received packet is not an ICMP echo reply\n");
		exit(1);
	}
	
	inet_ntop(AF_INET, &((struct sockaddr_in *)addr->ai_addr)->sin_addr, ip, INET_ADDRSTRLEN),
	printf("%zd bytes from %s: icmp_seq=%d ttl=%d time=%.3lf ms\n",
		size,
		ip,
		icmp_packet->un.echo.sequence,
		ip_packet->ttl,
		mtime);
}
