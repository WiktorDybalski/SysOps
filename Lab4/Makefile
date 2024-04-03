# Compiler used is gcc
CXX = gcc
# Compiler flags: -Wall (enable all warnings), -Wextra (enable extra warnings), -O0 (disable optimization), -g (include debugging information)
CFLAGS = -Wall -Wextra -O0 -g

.PHONY: all test1 task1 test2 task2 clean

all: test1 test2

#Task1
test1: task1

task1:
	$(CXX) $(CFLAGS) -o task1 task1.c
	./task1 7

#Task2
test2: task2

task2:
	$(CXX) $(CFLAGS) -o task2 task2.c
	./task2 /home/wiktor/CLionProjects/SysOps/Lab4
clean:
	rm -f task1 task2
