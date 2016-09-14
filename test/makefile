CC=gcc
CFLAGS=-I.

SRC_DIR=../src

out: $(SRC_DIR)/vc_server.c $(SRC_DIR)/vc_server.h $(SRC_DIR)/vcp.c $(SRC_DIR)/vcp.h $(SRC_DIR)/volume_controller.c $(SRC_DIR)/volume_controller.h 		
	$(CC) -Wall -g main.c $(SRC_DIR)/vc_server.c $(SRC_DIR)/vc_server.h $(SRC_DIR)/vcp.c $(SRC_DIR)/vcp.h $(SRC_DIR)/volume_controller.c $(SRC_DIR)/volume_controller.h -o out -lasound -lm

clean:
	rm -rf *.o out
