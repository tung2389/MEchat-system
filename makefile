CLIENT_NAME := chat_client
SERVER_NAME := chat_server

CDIR=client/
SDIR=server/
CPATH=$(CDIR)client.c
SPATH=$(SDIR)server.c

all: client server

headers := socket_util.h util.h shared.h
util := socket_util.o util.o shared.o

client: $(CDIR)client.o $(CDIR)client_helper.o $(util)
	gcc $(CDIR)client.o $(CDIR)client_helper.o $(util) -o $(CLIENT_NAME)

client.o: $(CDIR)client.c $(headers)
	gcc -c $(CDIR)client.c -o $(CDIR)client.o

client_helper.o: $(CDIR)client_helper.c $(headers)
	gcc -c $(CDIR)client_helper.c -o $(CDIR)client_helper.o

server: $(SDIR)server.o $(SDIR)server_helper.o $(util)
	gcc -pthread $(SDIR)server.o $(SDIR)server_helper.o $(util) -o $(SERVER_NAME)

server.o: $(SDIR)server.c $(SDIR)server_helper.h $(headers)	 
	gcc -c $(SDIR)server.c -o $(SDIR)server.o

server_helper.o: $(SDIR)server_helper.c $(SDIR)server_helper.h shared.h socket_util.h
	gcc -c $(SDIR)server_helper.c -o $(SDIR)server_helper.o

shared.o: shared.c
	gcc -c shared.c

util.o: util.c
	gcc -c util.c

socket_util.o: socket_util.c shared.h
	gcc -c socket_util.c

clean:
	rm *.o client/*.o server/*.o