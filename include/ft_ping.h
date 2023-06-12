#ifndef FTPING_FTPING_H_
#define FTPING_FTPING_H_

#include <stdbool.h>
#include <stdint.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "miniarg.h"
#include "libft/ft_lst.h"

#define PING_PKT_SIZE 84
#define PING_SLEEP_RATE 1000000
#define RECV_TIMEOUT 1

struct arguments {
	t_list *hosts;
	bool verbose;
	uint16_t count;
};

struct ping_pkt {
	struct iphdr iphdr;
	struct icmphdr icmphdr;
	char payload[PING_PKT_SIZE - sizeof(struct icmphdr) - sizeof(struct iphdr)];
};

struct ping_stat {
	double tmin;
	double tmax;
	double tsum;
	double tsumsq;
};

struct progconf {
	struct arguments args;
	bool loop;
};

extern const char *progname;

extern struct progconf progconf;

// args.c
int parse_opt(int key, const char *arg, struct marg_state *state);
	
// error.c
void error(const char *fmt, ...);
void error_exit(const int err, const char *fmt, ...);

// pkt.c
void set_iphdr(void *pkt, in_addr_t daddr);
void set_payload(void *pkt);
void set_icmphdr(void *pkt, uint16_t seq);
	
// ping.c
void ping(void *host);

// pong.c
void pong(const int sockfd, struct ping_stat *const stat, const char *const host);

// dns.c
struct addrinfo *get_host_info(const char *const host, int family);

// signal.c
void sig_int(int sig);
void sig_alarm(int sig);

// utils.c
double ping_sqrt(double n);

#endif // FTPING_FTPING_H_
