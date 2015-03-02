#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "netflow5.h"




static int netflow5_valid_pkt(const char* pkt_buf, int pkt_len) {
	int record_count;
	struct netflow5_header* header;

	if ((pkt_len - NETFLOW5_HEADER_SIZE) % NETFLOW5_RECORD_SIZE != 0) {
		printf("Received bad netflow v5 packet.\n");
		return 0;
	}

	record_count = (pkt_len - NETFLOW5_HEADER_SIZE) / NETFLOW5_RECORD_SIZE;

	header = (struct netflow5_header*) pkt_buf;

	if (ntohs(header->count) != record_count) {
		printf("Received bad netflow v5 packet.\n");
		return 0;
	}

	return record_count;
}


static void netflow5_handle_record(struct netflow5_header* header, struct netflow5_record* record) {
	uint32_t octets;
	uint16_t src_port;
	uint16_t dst_port;
	char src_addr[INET_ADDRSTRLEN];
	char dst_addr[INET_ADDRSTRLEN];

	octets = ntohl(record->d_octets);
	src_port = ntohs(record->src_port);
	dst_port = ntohs(record->dst_port);

	inet_ntop(AF_INET, &record->src_addr, src_addr, sizeof(src_addr));
	inet_ntop(AF_INET, &record->dst_addr, dst_addr, sizeof(dst_addr));


	printf("{ "
	         "\"src_adr\":\"%s\", "
	         "\"src_port\":\"%i\", "
	         "\"dst_addr\":\"%s\", "
	         "\"dst_port\":\"%i\", "
	         "\"delta_bytes\":\"%i\" "
	       "}\n",
		src_addr, src_port, dst_addr, dst_port, octets);
	fflush(stdout);
}


static void netflow5_handle_pkt(const char* pkt_buf, int record_count) {
	struct netflow5_header* header;
	struct netflow5_record* record;
	const char* cur_record_ptr;

	header = (struct netflow5_header*) pkt_buf;

	cur_record_ptr = pkt_buf + NETFLOW5_HEADER_SIZE;

	for (int i = 0; i < record_count; i++) {
		record = (struct netflow5_record*) cur_record_ptr;
		netflow5_handle_record(header, record);
		cur_record_ptr += NETFLOW5_RECORD_SIZE;
	}
}


void netflow5_recv_pkt(const char* pkt_buf, int pkt_len, struct sockaddr_in* src_addr) {
	int record_count;

	record_count = netflow5_valid_pkt(pkt_buf, pkt_len);
	if (record_count > 0) 
		netflow5_handle_pkt(pkt_buf, record_count);
}
