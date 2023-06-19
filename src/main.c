#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>

#include "miniarg.h"
#include "libft/ft_lst.h"

#include "ft_ping.h"

// Arguments
const char *marg_program_version = "ft_ping 0.0.1";
const char *marg_program_bug_address = "<me@izenynn.com>";

static char doc[] = "Send ICMP ECHO_REQUEST packets to network hosts.";
static char args_doc[] = "HOST ...";

static struct marg_option options[] = {
	{MARG_GRP, NULL, NULL, 0, "Options controlling ICMP request types:"},

	{MARG_GRP, NULL, NULL, 0, "Options valid for all request types:"},
	{'c', "count", NULL, OPTION_ARG_REQUIRED, "stop after sending N packets"},
	{'i', "interval", NULL, OPTION_ARG_REQUIRED, "wait N seconds between sending each packet"},
	{'n', "numeric", NULL, 0, "do not resolve host addresses"},
	{ARG_TTL, "ttl", NULL, OPTION_ARG_REQUIRED, "specify N as time-to-live"},
	{'v', "verbose", NULL, 0, "verbose output"},
	{'w', "timeout", NULL, OPTION_ARG_REQUIRED, "stop after N seconds"},
	{'W', "linger", NULL, OPTION_ARG_REQUIRED, "number of seconds to wait for response"},

	{MARG_GRP, NULL, NULL, 0, "Options valid for --echo requests:"},
	{'f', "flood", NULL, 0, "flood ping"},
	{'l', "preload", NULL, OPTION_ARG_REQUIRED, "send N packets as fast as possible before falling into normal made of behavior"},
	{'p', "pattern", NULL, OPTION_ARG_REQUIRED, "fill ICMP packet with given pattern"},
	{'s', "size", NULL, OPTION_ARG_REQUIRED, "send N data octets"},

	{0}
};

static struct marg marg = {options, parse_opt, args_doc, doc};

// Ping

struct progconf progconf = {
	.args = {
		.hosts = NULL,
		.verbose = false,
		.count = UINT16_MAX,
		.interval = PING_SLEEP_RATE
	},
	.loop = true
};

int main(int argc, char *argv[])
{
	// Arguments
	marg_parse(&marg, argc, argv, &progconf.args);

	// Signals
	if (signal(SIGINT, sig_int) == SIG_ERR) {
		log_exit(EX_OSERR, "%s", strerror(errno));
	}

	// Ping
	ft_lstiter(progconf.args.hosts, ping);

	// Clean up
	ft_lstclear(&progconf.args.hosts, NULL);

	return EX_OK;
}
