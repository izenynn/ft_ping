#ifndef FTPING_FTPING_H_
#define FTPING_FTPING_H_

#include <stdbool.h>
#include <netdb.h>

#include "miniarg.h"
#include "libft/ft_lst.h"

struct arguments {
	t_list *args;
	bool verbose;
	int count;
};

extern const char *progname;

// args.c
int parse_opt(int key, const char *arg, struct marg_state *state);
	
// error.c
void error_exit(const int err, const char *fmt, ...);

// ping.c
void send_ping(int sockfd, struct addrinfo *addr);

// pong.c
void receive_pong(int sockfd);

// dns.c
struct addrinfo *get_host_info(char* host, int family);

#endif // FTPING_FTPING_H_
