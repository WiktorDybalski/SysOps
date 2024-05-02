CXX = gcc
CFLAGS = -W -Wextra -g -lrt

all: run

run:
	$(CXX) $(CFLAGS) -o printer printer.c
	$(CXX) $(CFLAGS) -o client client.c

clean:
	rm -f printer client