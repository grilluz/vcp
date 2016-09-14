/*
 * Volume Control Protocol
 *
 * +----------------------+
 * | TYPE | LENGTH | DATA |
 * +----------------------+
 *
 * -TYPE:
 *
 * ---INFO:
 * 	request : DATA = [string] "hostname"
 * 	response: 'struct vc_server.hostname'
 * 	
 * 	request : DATA = [string] "address"
 * 	response: 'struct vc_server.ip_addr'
 * 
 * ---VOLUME:
 * 	request : LENGTH  = 0 request to get volume on the server
 * 	response: DATA = [long] volume
 * 	
 * 	request : LENGTH != 0 request to set volume with the value in DATA
 *	response: LENGTH = 0
 *
 *  On success, the response has always the same type of the request.
 *  If an error occours, the response type will be ERROR.
 *
 *  ---ERROR: (only response)
 *  	"invalid data":
 *  	"invalid type"             :
 *  	...  	
 */

#ifndef VCP_H
#define VCP_H

//measures are in bit
#define TYPE_LEN     3
#define LENGTH_LEN   8  //can represent max 64 values
#define MAX_DATA_LEN 65

typedef enum type {
	INFO = 0,
	VOLUME,
	ERROR
} vcp_type;

typedef struct _vcp_packet {
	vcp_type type;
	int length;
	char *data;
} vcp_packet; 

/*
 * convert int type in char array
 * return result in bin
 */
void vc_int_to_bin(int value, char *bin, int len);

void vc_bin_to_int(int *value, const char *bin, int len);

void vc_type_to_str(int value, char *str);

/*
 * data can be at max DATA_LEN large
 */
void init_vc_packet(vcp_packet *packet, vcp_type type, int length, const char *data);

void destroy_vc_packet(vcp_packet *packet);

#endif
