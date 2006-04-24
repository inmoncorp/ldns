/*
 * mx is a small programs that prints out the mx records
 * for a particulary domain
 * (c) NLnet Labs, 2005
 * See the file LICENSE for the license
 */

#include "config.h"

#include <ldns/dns.h>

int
main(int argc, char *argv[])
{
	ldns_pkt *q;	
	ldns_pkt *a;
	ldns_resolver *r;
	ldns_rr *rr;
	ldns_rr *soa;
	ldns_status s;

	q = ldns_pkt_new();
	r = ldns_resolver_new();

	(void)ldns_rr_new_frm_str(&rr, "localhost IN A 127.0.0.1", 0, NULL, NULL);
	(void)ldns_resolver_push_nameserver_rr(r, rr);
	(void)ldns_resolver_set_port(r, 5353);
	(void)ldns_resolver_set_usevc(r, true);

	/* setup a packet so that it is an reply */
	ldns_pkt_set_rcode(q, LDNS_PACKET_QUERY);
	(void)ldns_rr_new_frm_str(&rr, 
			"miek.nl IN IXFR", 0, NULL, NULL);
	(void)ldns_pkt_push_rr(q, LDNS_SECTION_QUESTION, rr);
	/* ldns_pkt_set_random_id(q); */

	/* next add some rrs, with SOA stuff so that we mimic or ixfr reply */
	(void)ldns_rr_new_frm_str(&soa, 
			"miek.nl IN SOA miek@miek.nl elektron.atoom.net 11 1 0 0 0", 0, NULL, NULL);

	(void)ldns_rr_new_frm_str(&rr, "miek.nl IN A 127.0.0.1", 0, NULL, NULL);

	/* compose the ixfr pkt */
	(void)ldns_pkt_push_rr(q, LDNS_SECTION_ANSWER, soa);
	(void)ldns_pkt_push_rr(q, LDNS_SECTION_ANSWER, rr);
	(void)ldns_pkt_push_rr(q, LDNS_SECTION_ANSWER, soa);
	
	ldns_pkt_print(stdout, q);

	fprintf(stderr, "\n** sending **\n");

	if ((s = ldns_resolver_send_pkt(&a, r, q)) == LDNS_STATUS_OK) {
		fprintf(stderr, "** recieved **\n");
		ldns_pkt_print(stdout, a);
	} else {
		fprintf(stderr, "** FAILURE: %s **\n", 
				ldns_get_errorstr_by_id(s));
	}
        return 0;
}