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
#include <errno.h>

#include "libft/ft_mem.h"
#include "libft/ft_fd.h"

#include "ft_ping.h"

static void send_pkt(int sockfd, struct addrinfo *addr, uint16_t seq)
{
	struct ping_pkt pkt;
	ssize_t err;

	set_iphdr(&pkt, ((struct sockaddr_in *)addr->ai_addr)->sin_addr.s_addr);
	set_payload(&pkt);
	set_icmphdr(&pkt, seq);

	/*
	 * IP_HDRINCL must be set on the socket so that
	 * the kernel does not attempt to automatically add
	 * a default ip header to the packet
	 */
	int optval = 1;
	err = setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));
	if (err == -1)
		log_pexit(EXIT_FAILURE, "setsockopt");

	err = sendto(sockfd, &pkt, sizeof(struct ping_pkt), 0,
		     addr->ai_addr, addr->ai_addrlen);
	if (err <= 0)
		log_pexit(EXIT_FAILURE, "sendto");

	++progconf.ping_num_xmit;
}

static void ping_init(char *host, struct addrinfo **addr, int *sockfd)
{
	struct timeval tv_out;
	int err;

	*addr = get_host_info(host, AF_INET);
	if (*addr == NULL)
		exit(EXIT_FAILURE);

	*sockfd = socket((*addr)->ai_family, (*addr)->ai_socktype,
			 (*addr)->ai_protocol);
	if (*sockfd < 0)
		log_pexit(EXIT_FAILURE, "socket");

	tv_out.tv_sec = RECV_TIMEOUT;
	tv_out.tv_usec = 0;
	err = setsockopt(*sockfd, SOL_SOCKET, SO_RCVTIMEO,
			 (const char*)&tv_out, sizeof tv_out);
	if (err == -1)
		log_pexit(EXIT_FAILURE, "setsockopt");

	progconf.ping_num_xmit = 0;
	progconf.ping_num_recv = 0;
	inet_ntop(AF_INET, &((struct sockaddr_in *)(*addr)->ai_addr)->sin_addr,
		  progconf.host, INET_ADDRSTRLEN);
	// FIXME
	reverse_dns(*addr);
	printf("REVERSE DNS: %s\n", progconf.rhost);
}

static void ping_hdrmsg(char *host)
{
	printf("PING %s (%s): %lu data bytes",
		(char *)host,
		progconf.host,
		sizeof(((struct ping_pkt *)0)->payload));
	if (progconf.args.verbose) {
		pid_t pid = getpid();
		printf(", id 0x%04x = %d", pid, pid);
	}
	printf("\n");
}

static void ping_statmsg(struct ping_stat *const stat)
{
	int perc = progconf.ping_num_xmit > 0
		   ? (int)((progconf.ping_num_xmit - progconf.ping_num_recv)
		     / progconf.ping_num_xmit * 100)
		   : 0;

	fflush(stdout);
	printf("--- %s ping statistics ---\n", progconf.host);
	printf("%zu packets transmitted, ", progconf.ping_num_xmit);
	printf("%zu packets received, ", progconf.ping_num_recv);
	printf("%d%% packet loss", perc);
	printf("\n");

	if (progconf.ping_num_recv > 0) {
		double total = (double)progconf.ping_num_recv;
		double avg = stat->tsum / total;
		double stddev = ping_sqrt(stat->tsumsq / total - avg * avg);
		printf("round-trip min/avg/max/stddev = %.3lf/%.3lf/%.3lf/%.3lf ms\n",
		       stat->tmin,
		       avg,
		       stat->tmax,
		       stddev);
	}
}

void ping(void *host)
{
	uint16_t seq = 0;
	struct addrinfo *addr;
	int sockfd;
	struct ping_stat stat = {
		.tmin = DBL_MAX,
		.tmax = DBL_MIN,
		.tsum = 0,
		.tsumsq = 0
	};

	ping_init(host, &addr, &sockfd);
	ping_hdrmsg(host);
	while (progconf.loop) {
		send_pkt(sockfd, addr, seq);
		pong(sockfd, &stat);
		++seq;
		if (!progconf.loop || seq >= progconf.args.count)
			break;
		usleep(progconf.args.interval);
	}
	ping_statmsg(&stat);
	
	freeaddrinfo(addr);
	close(sockfd);
}
