client: client.o socket_util.o
	gcc client.o socket_util.o -o client

client.o: client.c socket_util.h
	gcc -c client.c

server: server.o socket_util.o
	gcc server.o socket_util.o -o server

server.o: server.c socket_util.h	
	gcc -c server.c

socket_util.o: socket_util.c socket_util.h
	gcc -c socket_util.c

clean:
	rm *.o client server