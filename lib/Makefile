# コンパイラ設定
CC = gcc
CFLAGS = -Wall -O2 -I../include

# OS判定（Windowsの場合はMinGW想定）
ifeq ($(OS),Windows_NT)
    LDFLAGS = -lws2_32
else
    LDFLAGS =
endif
AR = ar rcs

SRC = ipc.c
OBJ = $(SRC:.c=.o)
LIB = libipc.a

.PHONY: all clean

all: $(LIB)

$(OBJ): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJ)
	$(AR) $@ $^

clean:
	rm -f $(OBJ) $(LIB)