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

const char *progname = NULL;

const char *marg_program_version = "ft_ping 0.0.1";
const char *marg_program_bug_address = "<me@izenynn.com>";

static char doc[] = "Send ICMP ECHO_REQUEST packets to network hosts.";
static char args_doc[] = "HOST ...";

static struct marg_option options[] = {
	{'v', "verbose", NULL, 0, "verbose output"},
	{'c', "count", NULL, OPTION_ARG | OPTION_ARG_REQUIRED, "stop after sending N packets"},
	{0}
};

static struct marg marg = {options, parse_opt, args_doc, doc};

// Ping

struct progconf progconf = {
	.args = {
		.hosts = NULL,
		.verbose = false,
		.count = UINT16_MAX
	},
	.loop = true
};

int main(int argc, char *argv[])
{
	// Arguments
	progname = argv[0];
	marg_parse(&marg, argc, argv, &progconf.args);

	// Signals
	if (signal(SIGINT, sig_int) == SIG_ERR || signal(SIGALRM, sig_alarm) == SIG_ERR) {
		error_exit(EX_OSERR, "%s", strerror(errno));
	}

	// Ping
	ft_lstiter(progconf.args.hosts, ping);

	// Clean up
	ft_lstclear(&progconf.args.hosts, NULL);

	return EX_OK;
}
