CC=g++
CFLAGS=-c -Wall

all: server

server: server.o
	$(CC) server.o -o server

server.o: server.cpp
	$(CC) $(CFLAGS) server.cpp

clean:
	rm -rf *.o server
