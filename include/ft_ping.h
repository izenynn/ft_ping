#ifndef FTPING_FTPING_H_
#define FTPING_FTPING_H_

#include <stdbool.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "miniarg.h"
#include "libft/ft_lst.h"

#define PKT_SIZE 84

#define RECV_TIMEOUT 1

struct arguments {
	t_list *args;
	bool verbose;
	int count;
};

struct ping_pkt {
	struct icmphdr hdr;
	char payload[PKT_SIZE - sizeof(struct icmphdr)];
}

extern const char *progname;

// args.c
int parse_opt(int key, const char *arg, struct marg_state *state);
	
// error.c
void error(const char *fmt, ...);
void error_exit(const int err, const char *fmt, ...);

// ping.c
void send_ping(int sockfd, struct addrinfo *addr);

// pong.c
void receive_pong(int sockfd);

// dns.c
struct addrinfo *get_host_info(char* host, int family);

// signal.c
void sig_int(int sig);
void sig_alarm(int sig);

#endif // FTPING_FTPING_H_
