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
