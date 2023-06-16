#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "ft_ping.h"

void ping_exit(const int err, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "%s: ", progname);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	exit(err);
}

void ping_error(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "%s: ", progname);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
}

void ping_perror(const char *s)
{
	ping_error("%s: %s", s, strerror(errno));
}

void ping_pexit(const int err, const char *s)
{
	ping_perror(s);
	exit(err);
}
