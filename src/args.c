#include <string.h>
#include <errno.h>
#include <sysexits.h>

#include "miniarg.h"
#include "libft/ft_nbr.h"
#include "libft/ft_lst.h"

#include "ft_ping.h"

int parse_opt(int key, const char *arg, struct marg_state *state)
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

