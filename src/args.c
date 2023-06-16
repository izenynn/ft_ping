#include <string.h>
#include <errno.h>
#include <sysexits.h>
#include <limits.h>

#include "miniarg.h"
#include "libft/ft_nbr.h"
#include "libft/ft_lst.h"
#include "libft/ft_char.h"

#include "ft_ping.h"

static bool isnum(const char *s)
{
	const char *p;

	for (p = s; *p != '\0'; ++p) {
		if (ft_isdigit(*p) == 0)
			return false;
	}
	return true;
}

int parse_opt(int key, const char *arg, struct marg_state *state)
{
	struct arguments *args = state->input;
	long count;
	t_list *new;

	switch (key) {
	case 'v':
		args->verbose = true;
		break;
	case 'c':
		if (isnum(arg) == false)
			ping_exit(marg_err_exit_status, "invalid value ('%s')", arg);
		count = ping_strtol(arg, NULL, 10);
		if (((count == LONG_MIN || count == LONG_MAX) && errno == ERANGE)
		    || (count < 1 || count > UINT16_MAX))
			ping_exit(marg_err_exit_status, "invalid argument ('%s') out range: %d - %d", arg, 0, UINT16_MAX);
		args->count = (uint16_t)count;
		break;
	case MARG_KEY_ARG:
		new = ft_lstnew((void *)arg);
		if (new == NULL) {
			ping_exit(EX_OSERR, "%s", strerror(errno));
		}
		ft_lstadd_back(&args->hosts, new);
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
