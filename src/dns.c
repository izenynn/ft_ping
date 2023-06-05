#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct addrinfo *get_host_info(char* host, int family)
{
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = family;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;

	if (getaddrinfo(host, NULL, &hints, &res) != 0) {
		perror("getaddrinfo() error");
		exit(1);
	}
	return res;
}
