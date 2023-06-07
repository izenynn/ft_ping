#include <signal.h>

#include "ft_ping.h"

void sig_int(int sig)
{
	(void)sig;
	loop = 0;
}

void sig_alarm(int sig)
{
	(void)sig;
}
