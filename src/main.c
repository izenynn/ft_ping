#include <stdio.h>
#include <stdlib.h>
#include "miniarg.h"
#include "libft/ft_nbr.h"
#include "ft_ping.h"

const char *marg_program_version = "ft_ping 0.0.1";
const char *marg_program_bug_address = "<me@izenynn.com>";

static char doc[] = "Send ICMP ECHO_REQUEST packets to network hosts.";
static char args_doc[] = "HOST";

static struct marg_option options[] = {
	{'v', "verbose", NULL, 0, false, "verbose output"},
	{'c', "count", NULL, OPTION_ARG | OPTION_ARG_REQUIRED, false, "stop after sending N packets"},
	{0}
};

static int parse_opt(int key, const char *arg, struct marg_state *state)
{
	struct arguments *args = state->input;

	switch (key) {
	case 'v':
		args->verbose = true;
		break;
	case 'c':
		args->count = ft_atoi(arg);
		break;
	case MARG_KEY_ARG:
		if (state->arg_num >= 1) {
			marg_error(state, "too many arguments");
		}
		args->args[state->arg_num] = arg;
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

int main(int argc, char *argv[])
{
	struct arguments args = {
		.args[0] = NULL,
		.verbose = false,
		.count = -1
	};

	marg_parse(&marg, argc, argv, &args);

	printf("Options:\n");
	printf("\t-v: %s\n", args.verbose ? "true" : "false");
	printf("\t-c: %d\n", args.count);
	printf("\tARG[0]: %s\n", args.args[0]);

	return 0;
}
