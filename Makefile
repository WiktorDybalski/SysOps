CXX = gcc
CFLAGS = -Wall -Wextra -g

.PHONY: all clean

all: countdown
countdown: countdown.c
	$(CXX) $(CFLAGS) -o countdown countdown.c
run: countdown
	./countdown
debug: countdown
	gdb -ex "break 6" -ex "run" ./countdown
clean:
	rm -f countdown