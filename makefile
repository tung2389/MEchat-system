all: client server

headers := socket_util.h shared.h

client: client.o socket_util.o
	gcc client.o socket_util.o -o client

client.o: client.c $(headers)
	gcc -c client.c

server: server.o socket_util.o
	gcc -pthread server.o socket_util.o -o server

server.o: server.c $(headers)	
	gcc -c server.c

socket_util.o: socket_util.c $(headers)
	gcc -c socket_util.c

clean:
	rm *.o client server