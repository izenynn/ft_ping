#ifndef FTPING_FTPING_H_
#define FTPING_FTPING_H_

#include <stdbool.h>
#include <stdint.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "miniarg.h"
#include "libft/ft_lst.h"

#define PKT_DATA_SIZE 56

#define PING_TTL 64
#define PING_INTERVAL_USEC 1000000 // 1 second
#define PING_LINGER_SEC 1

#define PING_FLOOD_INTERVAL 10000 // 10 ms
#define PING_FLOOD_LINGER_USEC 10000

// Define keys for long options that do not have short counterparts
enum {
	ARG_TTL = 256
};

enum pong_status {
	PONG_SUCCESS = 0,
	PONG_ERROR,
	PONG_TIMEOUT,
	PONG_RETRY
};

struct arguments {
	t_list *hosts;
	uint16_t count;
	bool is_interval;
	useconds_t interval;
	bool numeric;
	uint8_t ttl;
	bool verbose;
	time_t timeout;
	time_t linger;
	bool flood;
	uint16_t preload;
	bool is_pattern;
	const unsigned char *pattern;
	uint16_t size;
};

struct ping_pkt {
	struct iphdr iphdr;
	struct icmphdr icmphdr;
	char payload[];
};

struct ping_stat {
	double tmin;
	double tmax;
	double tsum;
	double tsumsq;
};

struct progconf {
	struct arguments args;
	struct timeval start;
	bool loop;

	char host[INET_ADDRSTRLEN];
	char rhost[NI_MAXHOST];
	size_t ping_num_xmit;
	size_t ping_num_recv;

	struct ping_pkt *pkt;
};

extern const char *progname;

extern struct progconf progconf;

// args.c
int parse_opt(int key, const char *arg, struct marg_state *state);
	
// log.c
void log_info(const char *fmt, ...);
void log_verbose(const char *fmt, ...);
void log_error(const char *fmt, ...);
void log_exit(const int err, const char *fmt, ...);
void log_perror(const char *s);
void log_pexit(const int err, const char *s);

// pkt.c
void set_iphdr(void *pkt, in_addr_t daddr);
void set_payload(void *pkt);
void set_icmphdr(void *pkt, uint16_t seq);
	
// ping.c
void ping(void *host);

// pong.c
enum pong_status pong(const int sockfd, struct ping_stat *const stat);

// dns.c
struct addrinfo *get_host_info(const char *const host, int family);
int reverse_dns(struct addrinfo *addr);

// signal.c
void sig_int(int sig);

// utils.c
double ping_sqrt(double n);
long ping_strtol(const char *nptr, char **endptr, int base);

#endif // FTPING_FTPING_H_
