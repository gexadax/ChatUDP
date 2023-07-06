CC = g++
CFLAGS = -std=c++11

server: server.cpp
	$(CC) $(CFLAGS) server.cpp -o server

client: client.cpp
	$(CC) $(CFLAGS) client.cpp -o client

.PHONY: clean

clean:
	rm -f server client
