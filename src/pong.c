#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

void receive_pong(int sockfd)
{
	char buffer[1024];
	memset(buffer, 0, sizeof buffer);
	
	struct timeval start, end;
	gettimeofday(&start, NULL);
	
	if (recvfrom(sockfd, buffer, sizeof buffer, 0, NULL, NULL) <= 0) {
		perror("recvfrom() error");
		exit(1);
	}
	
	gettimeofday(&end, NULL);
	long mtime = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
	
	struct icmp* icmp_packet = (struct icmp*) buffer;
	if (icmp_packet->icmp_type != ICMP_ECHOREPLY) {
		fprintf(stderr, "Received packet is not an ICMP echo reply\n");
		exit(1);
	}
	
	printf("Received packet from server, RTT = %ld ms\n", mtime);
}

