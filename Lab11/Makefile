CXX = gcc
CFLAGS = -W -Wextra -g -lrt

all: compile

compile:
	$(CXX) $(CFLAGS) -o server server.c
	$(CXX) $(CFLAGS) -o client client.c

run:
	./server
clean:
	rm -f server client


# To run server and clients
# ./server 127.0.0.1 8080
# ./client 127.0.0.1 8080

# Client can:
# 1. List all clients
# LIST:
# 2. Send message to everyone
# 2ALL:Hello
# 3. Send message to specific client
# 2ONE:2:Hello
# 4. Log out
# STOP:

# Server pings connected clients sending PING and waiting for PONG
# Server has a limit of connected clients