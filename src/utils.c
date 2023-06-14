#include <errno.h>
#include <limits.h>

#include "libft/ft_char.h"

#include "ft_ping.h"

static double absolute(double n)
{
	if (n < 0) return -n;
	return n;
}

// Fast square root function using Newton-Raphson method
double ping_sqrt(double n)
{
	const double tolerance = 1.0E-7; // Set the tolerance level
	double guess = n / 2.0; // Start with some guess

	while (absolute(guess * guess - n) > tolerance) {
		guess = (guess + n / guess) / 2.0;
	}

	return guess;
}

// It's forbidden so...
long ping_strtol(const char *nptr, char **endptr, int base)
{
	const char *s = nptr;
	unsigned long acc;
	int c;
	unsigned long cutoff;
	int neg = 0, any, cutlim;

	do {
		c = *s++;
	} while (ft_isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+') {
		c = *s++;
	}

	if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0) {
		base = c == '0' ? 8 : 10;
	}

	cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
	cutlim = (int)(cutoff % (unsigned long)base);
	cutoff /= (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (ft_isdigit(c))
			c -= '0';
		else if (ft_isalpha(c))
			c -= ft_isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= (unsigned long)base;
			acc += (unsigned long)c;
		}
	}
	if (any < 0) {
		acc = neg ? (unsigned long)LONG_MIN : LONG_MAX;
		errno = ERANGE;
	} else if (neg) {
		acc = -acc;
	}

	if (endptr != NULL)
		*endptr = (char *) (any ? s - 1 : nptr);

	return ((long)acc);
}
