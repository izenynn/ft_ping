#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

static unsigned short checksum(void *b, int len) {
	unsigned short *buf = b;
	unsigned int sum = 0;
	unsigned short result;

	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if (len == 1)
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = (unsigned short)~sum;
	return result;
}

void send_ping(int sockfd, struct addrinfo *addr) {
	struct icmp header;
	memset(&header, 0, sizeof header);
	header.icmp_type = ICMP_ECHO;
	header.icmp_code = 0;
	header.icmp_id = (unsigned short)getpid();
	header.icmp_seq = 1;
	header.icmp_cksum = 0;
	header.icmp_cksum = checksum(&header, sizeof(header));

	if (sendto(sockfd, &header, sizeof header, 0, addr->ai_addr, addr->ai_addrlen) <= 0) {
		perror("sendto() error");
		exit(1);
	}
}
