#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sysexits.h>

#include "miniarg.h"
#include "libft/ft_nbr.h"
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

static int parse_opt(int key, const char *arg, struct marg_state *state)
{
	struct arguments *args = state->input;
	t_list *new;

	switch (key) {
	case 'v':
		args->verbose = true;
		break;
	case 'c':
		args->count = ft_atoi(arg);
		break;
	case MARG_KEY_ARG:
		new = ft_lstnew((void *)arg);
		if (new == NULL) {
			error_exit(EX_OSERR, "%s", strerror(errno));
		}
		ft_lstadd_back(&args->args, new);
		break;
	case MARG_KEY_END:
		if (state->arg_num < 1) {
			marg_error(state, "missing host operand");
		}
		break;
	default:
		return 1;
	}

	return 0;
}

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

	return 0;
}
