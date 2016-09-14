//before running enter 'ulimit -c unlimited' (it allows to create core files)

#include "../src/vcp.h"
#include <stdio.h>
#include <unistd.h>
#include "../src/vc_server.h"

int main() {
	int fd_sock;
	struct vc_server server;

	fd_sock = init_vc_server(&server, 5);

	start_vc_server(&server);

	close(fd_sock);

	return 0;
}
