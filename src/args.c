#include <unistd.h>
#include <sys/types.h>
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

static long handle_long(const char *arg, int base, long min, long max)
{
	long tmp;

	if (isnum(arg) == false)
		log_exit(marg_err_exit_status, "invalid value ('%s')", arg);
	tmp = ping_strtol(arg, NULL, base);
	if (((tmp == LONG_MIN || tmp == LONG_MAX) && errno == ERANGE)
	    || (tmp < min || tmp > max))
		log_exit(marg_err_exit_status, "invalid argument ('%s') out range: %ld - %ld", arg, min, max);
	return tmp;
}

int parse_opt(int key, const char *arg, struct marg_state *state)
{
	struct arguments *args = state->input;
	long tmp;
	t_list *new;

	switch (key) {
	case 'c':
		args->count = (uint16_t)handle_long(arg, 10, 1, UINT16_MAX);
		break;
	case 'i':
		tmp = handle_long(arg, 10, 0, UINT_MAX / 1000000);
		args->interval = (useconds_t)(tmp * 1000000);
		args->is_interval = true;
		break;
	case 'n':
		args->numeric = true;
		break;
	case ARG_TTL:
		args->ttl = (uint8_t)handle_long(arg, 10, 1, UINT8_MAX);
		break;
	case 'v':
		args->verbose = true;
		break;
	case 'w':
		args->timeout = (time_t)handle_long(arg, 10, 1, LONG_MAX);
		break;
	case 'W':
		args->linger = (time_t)handle_long(arg, 10, 1, LONG_MAX);
		break;
	case 'f':
		if (getuid() != 0)
			log_exit(EX_USAGE, "cannot flood (root only)");
		args->flood = true;
		break;
	case 'l':
		args->preload = (uint16_t)handle_long(arg, 10, 0, UINT16_MAX);
		break;
	case 'p':
		handle_long(arg, 16, 0, INT_MAX);
		args->pattern = arg;
		args->is_pattern = true;
		break;
	case 's':
		args->size = (uint16_t)handle_long(arg, 10, 0, UINT16_MAX);
		break;
	case MARG_KEY_ARG:
		new = ft_lstnew((void *)arg);
		if (new == NULL) {
			log_pexit(EX_OSERR, "error");
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
