CXX = gcc
CFLAGS = -Wall -Wextra -00 -g

.PHONY: all task1 task2 test1 test2

all: test1 test2

test1: task1

task1:
	$(CXX) $(CFLAGS) -o task1 task1.c
	./task1 none
	./task1 ignore
	./task1 handler
	./task1 mask


test2: task2

task2:
	$(gcc) $(CFLAGS) -o catcher catcher.c
	$(gcc) $(CFLAGS) -o sender sender.c
	./catcher
	./sender <PID> 1

clean:
	rm -f task1 task2



