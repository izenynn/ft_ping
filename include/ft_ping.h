#ifndef FTPING_FTPING_H_
#define FTPING_FTPING_H_

#include <stdbool.h>

#include "libft/ft_lst.h"

struct arguments {
	t_list *args;
	bool verbose;
	int count;
};

extern const char *progname;

// error.c
void error_exit(const int err, const char *fmt, ...);

#endif // FTPING_FTPING_H_
