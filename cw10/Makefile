#compilator's flags
FLAGS = -Wall

all: server.c client.c common.c
	gcc server.c common.c -pthread -o server.out
	gcc client.c common.c -o client.out

server:
	./server.out 5001 "./unixSocket.socket"

localclient:
	./client.out name1 local "./unixSocket.socket"

netclient:
	./client.out name2 net localhost 5001

.PHONY: clean
clean:
	-rm -f client.out
	-rm -f server.out