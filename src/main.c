#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "miniarg.h"
#include "libft/ft_lst.h"

#include "ft_ping.h"

const char *progname = NULL;

const char *marg_program_version = "ft_ping 0.0.1";
const char *marg_program_bug_address = "<me@izenynn.com>";

static char doc[] = "Send ICMP ECHO_REQUEST packets to network hosts.";
static char args_doc[] = "HOST ...";

static struct marg_option options[] = {
	{'v', "verbose", NULL, 0, false, "verbose output"},
	{'c', "count", NULL, OPTION_ARG | OPTION_ARG_REQUIRED, false, "stop after sending N packets"},
	{0}
};

static struct marg marg = {options, parse_opt, args_doc, doc};

int main(int argc, char *argv[])
{
	struct arguments args = {
		.args = NULL,
		.verbose = false,
		.count = -1
	};

	progname = argv[0];
	marg_parse(&marg, argc, argv, &args);

	// TODO ft_ping
	printf("pinging nothing...\n");
	
	ft_lstclear(&args.args, NULL);

	return EX_OK;
}
