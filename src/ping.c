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
#include <float.h>

#include "libft/ft_mem.h"
#include "libft/ft_fd.h"

#include "ft_ping.h"

static void send_pkt(int sockfd, struct addrinfo *addr, uint16_t seq) {
	struct ping_pkt pkt;

	set_iphdr(&pkt, ((struct sockaddr_in *)addr->ai_addr)->sin_addr.s_addr);
	set_payload(&pkt);
	set_icmphdr(&pkt, seq);

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
	static char ip[INET_ADDRSTRLEN] = {0};
	uint16_t seq = 0;
	struct addrinfo *addr;
	int sockfd;
	struct timeval tv_out;
	struct ping_stat stat = {
		.tmin = DBL_MAX,
		.tmax = DBL_MIN,
		.tsum = 0,
		.tsumsq = 0
	};

	addr = get_host_info(host, AF_INET);
	sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (sockfd < 0) {
		// TODO use the error.c functions! perror() is forbidden!
		perror("socket() error");
		exit(1);
	}
	tv_out.tv_sec = RECV_TIMEOUT;
	tv_out.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof tv_out);

	inet_ntop(AF_INET, &((struct sockaddr_in *)addr->ai_addr)->sin_addr, ip, INET_ADDRSTRLEN),
	printf("PING %s (%s): %lu data bytes\n",
		(char *)host,
		ip,
		sizeof(((struct ping_pkt *)0)->payload));

	while (progconf.loop) {
		send_pkt(sockfd, addr, seq);
		pong(sockfd, &stat, ip);
		++seq;
		if (!progconf.loop || seq >= progconf.args.count)
			break;
		usleep(PING_SLEEP_RATE);
	}

	freeaddrinfo(addr);
	close(sockfd);

	// TODO print stats
	printf("--- %s ping statistics ---\n", (char *)host);
	printf("%d packets transmitted, %d packets received, %d%% packet loss\n",
		seq,
		0,
		0);
	// TODO change seq for number of received packages
	if (seq > 0) {
		printf("round-trip min/avg/max/stddev = %.3lf/%.3lf/%.3lf/%.3lf ms\n",
			stat.tmin,
			stat.tsum / seq,
			stat.tmax,
			0.0);
	}
}
