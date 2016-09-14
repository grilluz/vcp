/*
 *  
 */

#include "vc_server.h"
#include "vcp.h"
#include "volume_controller.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void set_hostname(struct vc_server *server) {
	if(server != NULL) {
		gethostname(server->hostname, HOSTNAME_LEN);
	}
}

void set_ip_addr(struct vc_server *server, int fd_conn) {
	int err;
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	char addr_str[IP_ADDR_LEN];

	if(server == NULL && fd_conn < 0) {
		return;
	}

	err = getsockname(fd_conn, (struct sockaddr *)&addr, &addr_len);
	if(err < 0) {
		return;
	}
	inet_ntop(AF_INET, &(addr.sin_addr), addr_str, IP_ADDR_LEN);
	
	strcpy(server->ip_addr, addr_str);
}

int vc_read(int fd, vcp_packet *packet) {
	int tot = 0;
	ssize_t bytes = 0;
	char type[TYPE_LEN];
       	char length[LENGTH_LEN];
	int type_val, length_val;
	char *data;

	if(fd < 0 || packet == NULL) {
		return -1;
	}

	bytes = read(fd, type, TYPE_LEN);
	if(bytes != TYPE_LEN) {
		if(bytes == 0) {
			return 0;
		} else {
			printf("\nerror reading type\n");
			return -1;
		} 
	}
	tot += (int)bytes;	
	vc_bin_to_int(&type_val, type, TYPE_LEN);

	bytes = read(fd, length, LENGTH_LEN);
	if(bytes != LENGTH_LEN) {
		if(bytes == 0) {
			return 0;
		} else {
			printf("\nerror reading length: %d\n", bytes);
			return -1;
		} 
	}
	tot += (int)bytes;

	vc_bin_to_int(&length_val, length, LENGTH_LEN);

	data = (char*)malloc(sizeof(char) * length_val);
	bytes = read(fd, data, length_val);
	if(bytes != length_val) {
		if(bytes == 0) {
			return 0;
		} else {
			printf("\nerror reading data\n");
			return -1;
		}
	} 	
	tot += (int)bytes;


	init_vc_packet(packet, type_val, length_val, data);

	free(data);

	return tot;
}

int vc_write(int fd, const vcp_packet *packet) {
	ssize_t bytes = 0;
	char type_str[TYPE_LEN];
	char length_str[LENGTH_LEN];

	if(fd < 0 || packet == NULL) {
		return -1;
	}

	vc_int_to_bin(packet->type, type_str, TYPE_LEN);
	vc_int_to_bin(packet->length, length_str, LENGTH_LEN);

	bytes = write(fd, type_str, TYPE_LEN);
	bytes = write(fd, length_str, LENGTH_LEN);
	if(packet->data != NULL) {
		bytes = write(fd, packet->data, packet->length);
	}

	return bytes;
}

int init_vc_server(struct vc_server *server, int max_conn) {
	int fd_sock;
	struct sockaddr_in server_addr;
	
	if(server == NULL) {
		return -1;
	}

	server->max_conn = max_conn;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(VC_PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	
	fd_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(fd_sock < 0) {
		return -1;
	}
	server->fd = fd_sock;

	//set non blocking socket
	/*
	if(fcntl(fd_sock, F_SETFL, flags | O_NONBLOCK) < 0) {
		return -1;
	}*/

	if(bind(fd_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		return -1;
	}

	set_hostname(server);
	
	return fd_sock;
}

void start_vc_server(struct vc_server *server) {
	fd_set active_fds, read_fds;
	int fd_sock;
	int rc, i, new_conn_fd, max_connections, bytes;
	struct sockaddr_in client;
	socklen_t client_len;
	vcp_packet request, response;
	
	bool one_time_flag = true;

	struct mixer_controller mixer_ctrl;
	strcpy(mixer_ctrl.card, "default");
	strcpy(mixer_ctrl.selem_name, "Master");
	
	//init mixer interface
	volume_controller_open(&mixer_ctrl);
	if(mixer_ctrl.handle == NULL || mixer_ctrl.sid == NULL) {
		return;
	}


	if(server == NULL) {
		return;
	}

	fd_sock = server->fd;
	max_connections = server->max_conn;
	if(listen(fd_sock, max_connections) < 0) {
		return;
	}

	FD_ZERO(&active_fds);
	FD_ZERO(&read_fds);

	//add the fd of the server to the fd set
	//so that client can connect to it
	FD_SET(fd_sock, &active_fds);

	while(1) {
		//each loop copy active fds in read fds
		//to read data of active connections
		read_fds = active_fds;	

		//select set the file descriptor sets contained 
		//in the read descriptor set
		rc = select(FD_SETSIZE, &read_fds, NULL, NULL, NULL);

		if(rc < 0) {
			//error
			break;
		} else if(rc == 0) {
			//timeout expires
			break;
		} else {
			//rc now contais the number of ready file 
			//descriptors in descriptor sets
			
			//loop all ready file descriptors
			for(i = 0; i <= FD_SETSIZE && rc > 0; i++) {
				//look for descriptor set which file descriptor belogs to
				
				if(FD_ISSET(i, &read_fds)) {

					if(i == fd_sock) {
						//listened to a new connection

						client_len = sizeof(client);
						new_conn_fd = accept(fd_sock,(struct sockaddr *)&client, &client_len);
						if(new_conn_fd < 0) {
							//error
						} else {
							//add the new file descriptor among active sets
							FD_SET(new_conn_fd, &active_fds);

							printf("New connection %d\n", new_conn_fd);

							//set ip address of the struct serveronly one time
							if(one_time_flag) {
								set_ip_addr(server, new_conn_fd);
								printf("Ip: %s\n", server->ip_addr);
								one_time_flag = false;
							}
						}

					} else {
						//read data from client

						bytes = vc_read(i, &request);
						
						if(bytes < 0) {
							//error							
						} else if(bytes == 0) {
							//client closed the connection
							printf("Connection %d closed\n", i);

							//close the fd
							close(i);

							//remove the fd from active descriptor sets
							FD_CLR(i, &active_fds);
						} else {
							int c;

							//display request
							printf("\nRequest from %d:\n", i);
							printf("\ttype  : %d\n", (int)request.type);
							printf("\tlength: %d\n", request.length);
							printf("\tdata  : ");
							for(c = 0; c < request.length; c++) {
								printf("%c", request.data[c]);
							}

							handle(server, &request, &response, &mixer_ctrl);
							vc_write(i, &response);

							//display response
							printf("\nResponse to %d:\n", i);
							printf("\ttype  : %d\n", (int)response.type);
							printf("\tlength: %d\n", response.length);
							printf("\tdata  : ");
							for(c = 0; c < response.length; c++) {
								printf("%c", response.data[c]);
							}
							printf("\n");
						}
					rc--;
					} //end of else
				} //end IF 
			} //end FOR
		} //end IF
	} //end of WHILE

	destroy_vc_packet(&request);
	destroy_vc_packet(&response);	

	volume_controller_close(&mixer_ctrl);	
}

void handle(const struct vc_server *server, const vcp_packet *request, vcp_packet *response, const struct mixer_controller *controller) {
	int length;
	long volume;
	char volume_str[VOLUME_LEN];

	if(request == NULL || response == NULL || server == NULL) {
		return;
	}
	
	switch((int)request->type) {
		case INFO:
			if(strncmp(request->data, "hostname\r\n", request->length) == 0) {
				length = strlen(server->hostname);
				init_vc_packet(response, INFO, length, server->hostname);	

			} else if(strncmp(request->data, "address\r\n", request->length) == 0) {
				length = strlen(server->ip_addr);
				init_vc_packet(response, INFO, length, server->ip_addr);

			} else {
				const char *data = "invalid info request";
				init_vc_packet(response, ERROR, strlen(data), data);
			}		
			break;

		case VOLUME:
			length = request->length;
			
			if(length == 0) {
				get_volume(controller->elem, &volume);
				
				//volume range is 0~100, so there isn't loss of data in the cast
				vc_int_to_bin((int)volume, volume_str, VOLUME_LEN);
				
				init_vc_packet(response, VOLUME, VOLUME_LEN, volume_str);

			} else if (length > 0) {
				int volume_int;
				vc_bin_to_int(&volume_int, request->data, request->length);

				volume = volume_int;
				set_volume(controller->elem, volume);

				init_vc_packet(response, VOLUME, 0, NULL);

			} else {
				const char *data = "invalid volume request";
				init_vc_packet(response, ERROR, strlen(data), data);
			}
			break;

		default:
			{
			const char *data = "invalid type";
			init_vc_packet(response, ERROR, strlen(data), data);
			break;
			}	
	}
}
