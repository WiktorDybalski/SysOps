CXX = gcc
CFLAGS = -Wall -Wextra -O0 -g

.PHONY: all task1 test1 task2 test2 sender catcher

all: test1 test2

test1: task1

task1:
	$(CXX) $(CFLAGS) -o task1 task1.c
	./task1 ignore
	./task1 handler
	./task1 mask
	./task1 none

test2: task2

task2: sender catcher

catcher:
	$(CXX) $(CFLAGS) -o catcher catcher.c
	./catcher

sender:
	$(CXX) $(CFLAGS) -o sender sender.c

clean:
	rm -f task1 catcher sender



