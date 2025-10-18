# コンパイラ設定
CC = gcc
CFLAGS = -Wall -O2 -I../include
LDFLAGS = ../lib/libipc.a

# OS判定（Windowsの場合はMinGW想定）
ifeq ($(OS),Windows_NT)
    LDFLAGS += -lws2_32
endif

SERVER_SRC = server.c
SERVER_BIN = server
CLIENT_SRC = client.c
CLIENT_BIN = client

.PHONY: all clean

all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)