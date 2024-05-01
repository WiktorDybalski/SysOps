CXX = gcc
CFLAGS = -W -Wextra -g -lrt

all: run

run:
	$(CXX) $(CFLAGS) -o printer printer.c
	$(CXX) $(CFLAGS) -o client client.c
	./printer 4
	./client 2

clean:
	rm -f server client1 client2 client3 client4 client5