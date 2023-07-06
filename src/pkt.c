#include <stdint.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <unistd.h>

#include "libft/ft_mem.h"
#include "libft/ft_str.h"

#include "ft_ping.h"

static unsigned short checksum(void *b, int len) {
	unsigned short *buf = b;
	unsigned int sum = 0;
	unsigned short result;

	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if (len == 1)
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = (unsigned short)~sum;
	return result;
}

void set_iphdr(void *pkt, in_addr_t daddr)
{
	struct iphdr *hdr = &((struct ping_pkt *)pkt)->iphdr;

	ft_memset(hdr, 0, sizeof(struct iphdr));

	hdr->ihl = 5; // Size in 32-bit words
	hdr->version = 4;
	hdr->tos = 0;
	hdr->tot_len = htons(sizeof(struct ping_pkt) + progconf.args.size);
	hdr->id = htons((uint16_t)getpid());
	hdr->frag_off = 0;
	hdr->ttl = progconf.args.ttl;
	hdr->protocol = IPPROTO_ICMP;
	hdr->saddr = INADDR_ANY;
	hdr->daddr = daddr;
	hdr->check = checksum(hdr, sizeof(struct iphdr));
}

static char htob(const char *hex)
{
	char byte[3];
	ft_memcpy(byte, hex, 2);
	byte[2] = '\0';

	return (char)ping_strtol(byte, NULL, 16);
}

void set_payload(void *pkt)
{
	char *payload = ((struct ping_pkt *)pkt)->payload;
	char *i;
	const char *pattern = progconf.args.pattern;
	const char *j;
	intptr_t pattern_len;

	if (!progconf.args.is_pattern) {
		ft_memset(payload, 0, progconf.args.size);
		return;
	}

	pattern_len = (intptr_t)ft_strlen(pattern);

	j = pattern;
	for (i = payload; i - payload < progconf.args.size; ++i) {
		if (j - pattern >= pattern_len)
			j = pattern;
		*i = htob(j);
		j += 2;
	}
}

void set_icmphdr(void *pkt, uint16_t seq)
{
	struct icmphdr *hdr = &((struct ping_pkt *)pkt)->icmphdr;

	ft_memset(hdr, 0, sizeof(struct icmphdr));

	hdr->type = ICMP_ECHO;
	hdr->code = 0;
	hdr->un.echo.id = htons((uint16_t)getpid());
	hdr->un.echo.sequence = htons(seq);
	hdr->checksum = checksum(hdr, sizeof(struct icmphdr) + progconf.args.size);
}
