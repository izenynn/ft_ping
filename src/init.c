#include <stdio.h>

#include "ft_ping.h"

int init(void)
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

	return 0;
}
