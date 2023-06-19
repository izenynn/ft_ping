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
	long tmp;
	t_list *new;

	switch (key) {
	case 'c':
		if (isnum(arg) == false)
			log_exit(marg_err_exit_status, "invalid value ('%s')", arg);
		tmp = ping_strtol(arg, NULL, 10);
		if (((tmp == LONG_MIN || tmp == LONG_MAX) && errno == ERANGE)
		    || (tmp < 1 || tmp > UINT16_MAX))
			log_exit(marg_err_exit_status, "invalid argument ('%s') out range: %d - %d", arg, 0, UINT16_MAX);
		args->count = (uint16_t)tmp;
		break;
	case 'i':
		if (isnum(arg) == false)
			log_exit(marg_err_exit_status, "invalid value ('%s')", arg);
		tmp = ping_strtol(arg, NULL, 10);
		if (((tmp == LONG_MIN || tmp == LONG_MAX) && errno == ERANGE)
		    || (tmp < 0 || tmp > UINT_MAX / 1000000))
			log_exit(marg_err_exit_status, "invalid argument ('%s') out range: %d - %d", arg, 0, UINT_MAX / 1000000);
		args->interval = (useconds_t)(tmp * 1000000);
		break;
	case 'n':
		args->numeric = true;
		break;
	case 'v':
		args->verbose = true;
		break;
	case MARG_KEY_ARG:
		new = ft_lstnew((void *)arg);
		if (new == NULL) {
			log_exit(EX_OSERR, "%s", strerror(errno));
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
