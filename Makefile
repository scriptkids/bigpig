CFLAGS:=-Wall

bigpig: server_epoll.o tcp_listen.o log.o file_ctrl.o request.o
	gcc -o$@ $^

server1: server1.o tcp_listen.o
	gcc -o$@ $^

server_select: server_select.o tcp_listen.o log.o file_ctrl.o request.o
	gcc -o$@ $^

#	./server_epoll

server_select2: server_select2.o tcp_listen.o
	gcc -o$@ $^

client: client.o
	gcc -o$@ $^
clean:
	rm -f *.o
	rm -f server1 server_select server_select2 server_epoll
	rm -f client
	rm -f bigpig
