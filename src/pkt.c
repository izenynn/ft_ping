#include <stdint.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <unistd.h>

#include "libft/ft_mem.h"

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
	hdr->tot_len = htons(sizeof(struct ping_pkt));
	hdr->id = htons((uint16_t)getpid());
	hdr->frag_off = htons(0);
	hdr->ttl = 64;
	hdr->protocol = IPPROTO_ICMP;
	hdr->saddr = INADDR_ANY;
	hdr->daddr = daddr;
	hdr->check = checksum(hdr, sizeof(struct iphdr));
}

void set_payload(void *pkt)
{
	char *payload = ((struct ping_pkt *)pkt)->payload;

	ft_memset(payload, 0, sizeof(((struct ping_pkt *)0)->payload));
}

void set_icmphdr(void *pkt, uint16_t seq)
{
	struct icmphdr *hdr = &((struct ping_pkt *)pkt)->icmphdr;

	ft_memset(hdr, 0, sizeof(struct icmphdr));

	hdr->type = ICMP_ECHO;
	hdr->code = 0;
	hdr->un.echo.id = htons((uint16_t)getpid());
	hdr->un.echo.sequence = htons(seq);
	hdr->checksum = checksum(hdr, sizeof(struct icmphdr) + sizeof(((struct ping_pkt *)0)->payload));
}