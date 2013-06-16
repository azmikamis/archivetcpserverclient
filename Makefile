CC      = gcc

all: tcpserver.exe tcpclient.exe

tcpserver.exe: tcpserver.o
	$(CC) tcpserver.o -o tcpserver.exe -lws2_32

tcpclient.exe: tcpclient.o
	$(CC) tcpclient.o -o tcpclient.exe -lws2_32

tcpserver.o: tcpserver.c
	$(CC) -c tcpserver.c

tcpclient.o: tcpclient.c
	$(CC) -c tcpclient.c