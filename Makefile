SERVER = server
CLIENT = client

SERVER_S = server.c
CLIENT_S = client.c

CC=gcc

CFLAGS=-std=c99 -pedantic -Wextra -Wall -pthread

all: client server

client:
	$(CC) $(CFLAGS) $(CLIENT_S) -o client

server:
	$(CC) $(CFLAGS) $(SERVER_S) -o server

clean:
	rm server
	rm client
