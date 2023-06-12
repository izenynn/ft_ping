#include <signal.h>

#include "ft_ping.h"

void sig_int(int sig)
{
	(void)sig;
	progconf.loop = false;
}

void sig_alarm(int sig)
{
	(void)sig;
}
