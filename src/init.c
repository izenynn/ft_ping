#include <stdio.h>
#include <sysexits.h>
#include <signal.h>
#include <sys/time.h>

#include "ft_ping.h"

static void print_pattern(void)
{
	if (progconf.args.is_pattern) {
		printf("PATTERN: 0x");
		for (const char *p = progconf.args.pattern; *p != '\0';) {
			if (*(p + 1) == '\0') {
				printf("0%c", *p);
				break;
			} else {
				printf("%c%c", *p, *(p + 1));
			}
			p += 2;
		}
		printf("\n");
	}
}

int init(void)
{
	if (gettimeofday(&progconf.start, NULL))
		log_pexit(EX_OSERR, "gettimeofday");
	progconf.pkt = malloc(sizeof(struct ping_pkt) + progconf.args.size);
	if (progconf.pkt == NULL)
		log_pexit(EX_OSERR, "malloc");
	if (signal(SIGINT, sig_int) == SIG_ERR)
		log_pexit(EX_OSERR, "signal");

	print_pattern();

	return 0;
}

int finish(void)
{
	free(progconf.pkt);
	ft_lstclear(&progconf.args.hosts, NULL);

	return 0;
}
