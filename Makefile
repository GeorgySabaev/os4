all: client server
client: client.c DieWithError.c protocol.h
	gcc client.c DieWithError.c -o client
server: server.c DieWithError.c protocol.h
	gcc server.c DieWithError.c -o server