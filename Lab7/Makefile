CXX = gcc
CFLAGS = -W -Wextra -g -lrt

all: compile

compile:
	$(CXX) $(CFLAGS) -o server server.c
	$(CXX) $(CFLAGS) -o client1 client.c
	$(CXX) $(CFLAGS) -o client2 client.c
	$(CXX) $(CFLAGS) -o client3 client.c
	$(CXX) $(CFLAGS) -o client4 client.c
	$(CXX) $(CFLAGS) -o client5 client.c

run:
	./server
clean:
	rm -f server client1 client2 client3 client4 client5