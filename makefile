all: client server

headers := socket_util.h util.h shared.h
util := socket_util.o util.o shared.o

client: client.o $(util)
	gcc client.o $(util) -o client

client.o: client.c $(headers)
	gcc -c client.c

server: server.o server_helper.o $(util)
	gcc -pthread server.o server_helper.o $(util) -o server

server.o: server.c server_helper.h $(headers)	 
	gcc -c server.c

server_helper.o: server_helper.c shared.h
	gcc -c server_helper.c

shared.o: shared.c
	gcc -c shared.c

util.o: util.c
	gcc -c util.c

socket_util.o: socket_util.c shared.h
	gcc -c socket_util.c

clean:
	rm *.o client server