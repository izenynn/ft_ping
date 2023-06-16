#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "miniarg.h"

#include "ft_ping.h"

void log_info(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fprintf(stdout, "%s: ", marg_program_name);
	vfprintf(stdout, fmt, args);
	fprintf(stdout, "\n");
	va_end(args);
}

void log_verbose(const char *fmt, ...)
{
	va_list args;

	if (progconf.args.verbose) {
		va_start(args, fmt);
		fprintf(stdout, "%s: ", marg_program_name);
		vfprintf(stdout, fmt, args);
		fprintf(stdout, "\n");
		va_end(args);
	}
}

void log_error(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "%s: ", marg_program_name);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
}

void log_exit(const int err, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "%s: ", marg_program_name);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	exit(err);
}

void log_perror(const char *s)
{
	log_error("%s: %s", s, strerror(errno));
}

void log_pexit(const int err, const char *s)
{
	log_perror(s);
	exit(err);
}
