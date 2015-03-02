#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "netflow5.h"


#define	BUFSIZE 8192


void errh(char *s) {
	perror(s);
	exit(1);
}


int setup_socket(in_addr_t listen_ipaddr, uint16_t listen_port) {
	int sockfd;
	struct sockaddr_in sin;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		errh("socket()");

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = listen_ipaddr;
	sin.sin_port = listen_port;

	if (bind(sockfd, (struct sockaddr *) &sin, sizeof(sin)) == -1)
	    errh("bind()");

	return sockfd;
}


void mainloop(int netflow_sockfd) {
	char pkt_buf[BUFSIZE];
	int pkt_len;
	struct sockaddr_in src_addr;
	socklen_t plen = sizeof(struct sockaddr_in);
	uint16_t* version = NULL;

	while (1) {
		memset(pkt_buf, 0, BUFSIZE);

		if ((pkt_len = recvfrom(netflow_sockfd, pkt_buf, BUFSIZE, 0, (struct sockaddr *) &src_addr, &plen)) == -1) {
			perror("recvfrom()");
			continue;
		}

		// pkt_len bytes from src_addr are in pkt_buf, it's a whole packet
		// printf("Debug: processing new packet\n");

		version = (uint16_t*) pkt_buf;
		switch (ntohs(*version)) {
			case 5:
				netflow5_recv_pkt(pkt_buf, pkt_len, &src_addr);
				break;
			default:
				break;
		}
	}
}


int main(int argc, char *argv[]) {
	int opt;
	uint16_t bind_port = 2055;
	in_addr_t bind_addr = INADDR_ANY;
	int netflow_sockfd;

	while ((opt = getopt(argc, argv, "p:")) != -1) {
		switch ((char) opt) {
			case 'p':
				sscanf(optarg, "%" SCNu16, &bind_port);
				break;

			default:
				return -1;
		}
	}

	bind_port = htons(bind_port);
	printf("Binding to port: %i\n", ntohs(bind_port));

	netflow_sockfd = setup_socket(bind_addr, bind_port);
	mainloop(netflow_sockfd);
}
