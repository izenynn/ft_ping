#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "ft_ping.h"

struct addrinfo *get_host_info(const char *const host, int family)
{
	struct addrinfo hints;
	struct addrinfo *res;
	int err;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = family;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;

	err = getaddrinfo(host, NULL, &hints, &res);
	if (err != 0) {
		if (err == EAI_SYSTEM)
			log_error("looking up %s: %s", host, strerror(errno));
		else
			log_error("looking up %s: %s", host, gai_strerror(err));
		return NULL;
	}

	return res;
}

int reverse_dns(struct addrinfo *addr)
{
	int err;

	err = getnameinfo(addr->ai_addr, sizeof(struct sockaddr_in), progconf.rhost, sizeof(progconf.rhost), NULL, 0, NI_NAMEREQD);
	if (err != 0) {
		if (err == EAI_SYSTEM)
			log_error("looking up %s: %s", progconf.rhost, strerror(errno));
		else
			log_error("looking up %s: %s", progconf.rhost, gai_strerror(err));
		return 1;
	}
	return 0;
}
