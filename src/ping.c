#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>

#include "libft/ft_mem.h"
#include "libft/ft_fd.h"

#include "ft_ping.h"

static void send_pkt(int sockfd, struct addrinfo *addr) {
	struct ping_pkt pkt;

	set_iphdr(&pkt, ((struct sockaddr_in *)addr->ai_addr)->sin_addr.s_addr);
	set_payload(&pkt);
	set_icmphdr(&pkt, 0);

	/*
	 * IP_HDRINCL must be set on the socket so that
	 * the kernel does not attempt to automatically add
	 * a default ip header to the packet
	 */
	int optval = 1;
	setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));

	//ft_putmem_fd(&pkt, sizeof(struct ping_pkt), 1);
	if (sendto(sockfd, &pkt, sizeof(struct ping_pkt), 0, addr->ai_addr, addr->ai_addrlen) <= 0) {
		perror("sendto() error");
		exit(1);
	}
}

void ping(void *host)
{
	static char ip[INET_ADDRSTRLEN];
	struct addrinfo *addr;
	int sockfd;

	addr = get_host_info(host, AF_INET);
	sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (sockfd < 0) {
		perror("socket() error");
		exit(1);
	}

	inet_ntop(AF_INET, &((struct sockaddr_in *)addr->ai_addr)->sin_addr, ip, INET_ADDRSTRLEN),
	printf("PING %s (%s): %lu data bytes\n",
		(char *)host,
		ip,
		sizeof(((struct ping_pkt *)0)->payload));

	while (loop) {
		send_pkt(sockfd, addr);
		pong(sockfd, addr);
		usleep(PING_SLEEP_RATE);
	}

	freeaddrinfo(addr);
	close(sockfd);
}
