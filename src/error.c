#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "ft_ping.h"

void error_exit(const int err, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "%s: ", progname);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	exit(err);
}
