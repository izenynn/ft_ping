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

static void print_data(void *data)
{
	printf("  %s\n", (char *)data);
}
int main(int argc, char *argv[])
{
	struct arguments args = {
		.args = NULL,
		.verbose = false,
		.count = -1
	};

	progname = argv[0];
	marg_parse(&marg, argc, argv, &args);

	printf("Options:\n");
	printf(" -v: %s\n", args.verbose ? "true" : "false");
	printf(" -c: %d\n", args.count);
	printf(" ARGS:\n");
	ft_lstiter(args.args, print_data);

	ft_lstclear(&args.args, NULL);

	return EX_OK;
}
