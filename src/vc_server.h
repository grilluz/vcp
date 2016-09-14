#ifndef VC_SERVER_H
#define VC_SERVER_H

#include <arpa/inet.h>
#include "volume_controller.h"

#include "vcp.h"

#define VC_PORT 1996
#define HOSTNAME_LEN 64
#define IP_ADDR_LEN INET_ADDRSTRLEN

struct vc_server {
	char hostname[HOSTNAME_LEN];
	char ip_addr[INET_ADDRSTRLEN];
	int max_conn;
	int fd;	
};

void set_hostname(struct vc_server *server);

void set_ip_addr(struct vc_server *server, int fd_conn);

int vc_read(int fd, vcp_packet *packet);

int vc_write(int fd, const vcp_packet *packet);

int init_vc_server(struct vc_server *server, int max_conn);

void start_vc_server(struct vc_server *server);

void handle(const struct vc_server *server, const vcp_packet *request, vcp_packet *response, const struct mixer_controller *controller);

#endif
