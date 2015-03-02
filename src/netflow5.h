#ifndef NETFLOW5_H
#define NETFLOW5_H

#include <sys/types.h>
#include <netinet/in.h>


struct netflow5_header {
	uint16_t version;
	uint16_t count;
	uint32_t sys_uptime;
	uint32_t unix_secs;
	uint32_t unix_nsecs;
	uint32_t flow_sequence;
	uint8_t  engine_type;
	uint8_t  engine_id;
	uint16_t sampling_interval;
};


struct netflow5_record {
	uint32_t src_addr;
	uint32_t dst_addr;
	uint32_t nexthop_addr;
	uint16_t input_snmp;
	uint16_t output_snmp;
	uint32_t d_pkts;
	uint32_t d_octets;
	uint32_t sys_uptime_first;
	uint32_t sys_uptime_last;
	uint16_t src_port;
	uint16_t dst_port;
	uint8_t  pad1;
	uint8_t  tcp_flags;
	uint8_t  prot;
	uint8_t  tos;
	uint16_t src_as;
	uint16_t dst_as;
	uint8_t  src_mask;
	uint8_t  dst_mask;
	uint16_t pad2;
};


#define NETFLOW5_HEADER_SIZE sizeof(struct netflow5_header)
#define NETFLOW5_RECORD_SIZE sizeof(struct netflow5_record)


void netflow5_recv_pkt(const char* pkt_buf, int pkt_len, struct sockaddr_in* src_addr);


#endif
