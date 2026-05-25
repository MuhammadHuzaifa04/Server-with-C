CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -Iinclude
LDFLAGS=-lws2_32
SRC=main.c src/http.c src/request.c src/response.c src/router.c src/connection.c
OUT=server

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)

.PHONY: all run clean
