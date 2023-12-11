#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "miniarg.h"

#include "ft_ping.h"

static void log_format(FILE *output, const char *fmt, va_list args)
{
	fprintf(output, "%s: ", marg_program_name);
	vfprintf(output, fmt, args);
	fprintf(output, "\n");
}

void log_info(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	log_format(stdout, fmt, args);
	va_end(args);
}

void log_error(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	log_format(stderr, fmt, args);
	va_end(args);
}

void log_vinfo(const char *fmt, ...)
{
	va_list args;

	if (progconf.args.verbose) {
		va_start(args, fmt);
		log_format(stdout, fmt, args);
		va_end(args);
	}
}

void log_verror(const char *fmt, ...)
{
	va_list args;

	if (progconf.args.verbose) {
		va_start(args, fmt);
		log_format(stderr, fmt, args);
		va_end(args);
	}
}

void log_exit(const int err, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	log_format(stderr, fmt, args);
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
