BUILD_DIR := bin
SRC_DIR := src
OBJ_DIR := obj
FLAGS := -Wall
COMMON_DEP = $(SRC_DIR)/tftp_lib.c $(SRC_DIR)/utils.c
COMMON_DEP_O = $(OBJ_DIR)/tftp_lib.o $(OBJ_DIR)/utils.o
COMMON_H = $(SRC_DIR)/tftp_lib.h $(SRC_DIR)/utils.h

all: dir $(BUILD_DIR)/tftp_client $(BUILD_DIR)/tftp_server

$(OBJ_DIR)/tftp_lib.o: $(SRC_DIR)/tftp_lib.c $(COMMON_H)
	gcc $(FLAGS) -c $(SRC_DIR)/tftp_lib.c -o $(OBJ_DIR)/tftp_lib.o

$(OBJ_DIR)/utils.o: $(SRC_DIR)/utils.c $(SRC_DIR)/utils.c
	gcc $(FLAGS) -c $(SRC_DIR)/utils.c -o $(OBJ_DIR)/utils.o

$(OBJ_DIR)/tftp_server.o: $(SRC_DIR)/tftp_server.c $(COMMON_H)
	gcc $(FLAGS) -c $(SRC_DIR)/tftp_server.c -o $(OBJ_DIR)/tftp_server.o

$(OBJ_DIR)/tftp_client.o: $(SRC_DIR)/tftp_client.c $(COMMON_H)
	gcc $(FLAGS) -c $(SRC_DIR)/tftp_client.c -o $(OBJ_DIR)/tftp_client.o

$(BUILD_DIR)/tftp_client: $(OBJ_DIR)/tftp_client.o $(COMMON_DEP_O)
	gcc $(FLAGS) $(OBJ_DIR)/tftp_client.o $(COMMON_DEP_O) -o $(BUILD_DIR)/tftp_client

$(BUILD_DIR)/tftp_server: $(OBJ_DIR)/tftp_server.o $(COMMON_DEP_O)
	gcc $(FLAGS) $(OBJ_DIR)/tftp_server.o $(COMMON_DEP_O) -o $(BUILD_DIR)/tftp_server


dir:
	mkdir -p $(BUILD_DIR) $(SRC_DIR) $(OBJ_DIR)

clean:
	rm $(OBJ_DIR)/*.o
	rm -r $(BUILD_DIR)
