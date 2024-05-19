CC=gcc

CFLAGS=-pthread

SRC_DIR=src
BIN_DIR=bin

SERVER_SRCS=$(SRC_DIR)/book.c $(SRC_DIR)/server.c $(SRC_DIR)/transactions.c $(SRC_DIR)/user.c $(SRC_DIR)/utils.c
CLIENT_SRCS=$(SRC_DIR)/client.c

SERVER_OUT=$(BIN_DIR)/server
CLIENT_OUT=$(BIN_DIR)/client
BYPRODUCT=$(BIN_DIR)/*.bin $(BIN_DIR)/*.log

all: server client

server: $(SERVER_SRCS)
	$(CC) $(CFLAGS) $(SERVER_SRCS) -o $(SERVER_OUT)

client: $(CLIENT_SRCS)
	$(CC) $(CLIENT_SRCS) -o $(CLIENT_OUT)

clean:
	rm -f $(SERVER_OUT) $(CLIENT_OUT) $(BYPRODUCT)

.PHONY: all server client clean
