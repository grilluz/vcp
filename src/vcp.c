#include "vcp.h"

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

void vc_int_to_bin(int value, char *bin, int len) {
	int mask = 1, i;

	if(len < 1) {
		return;
	}

	for(i = len - 1; i >= 0; i--, mask <<= 1) {
		bin[i] = (value & mask) ? '1' : '0';
	}
}

void vc_bin_to_int(int *value, const char *bin, int len) {
	int i = 0;
	*value = 0;

	if(len < 1) {
		return;
	}

	for(i = 0; i < len; i++) {
		*value <<= 1;
		if(bin[i] == '1') {
			*value |= 1;
		} else {
			*value &= (INT_MAX - 1);
		}
	}
}

void vc_type_to_str(int value, char *str) {
	if(value < 0 || str == NULL) {
		return;
	}

	switch(value) {
		case 0:
			strcpy(str, "INFO");
			break;
		case 1:
			strcpy(str, "VOLUME");
			break;
		case 2:
			strcpy(str, "ERROR");
			break;
		default:
			break;
	}
}

void init_vc_packet(vcp_packet *packet, vcp_type type, int length, const char *data) {
	if(packet == NULL) {
		return;
	}
	
	packet->type = type;
	packet->length = length;

	printf("\nDBG len = %d\n", length);

	destroy_vc_packet(packet);

	if(length == 0) {
		//printf("DBG if length equals zero\n");
		packet->data = NULL;
	} else {
		packet->data = (char *)malloc(sizeof(char) * length);
		strcpy(packet->data, data); 	
	}
} 

void destroy_vc_packet(vcp_packet *packet) {
	if(packet == NULL || packet->data == NULL) {
		return;
	}

	free(packet->data);
}
