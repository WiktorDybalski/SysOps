# Compiler used is gcc
CXX = gcc
# Compiler flags: -Wall (enable all warnings), -Wextra (enable extra warnings), -O0 (disable optimization), -g (include debugging information)
CFLAGS = -Wall -Wextra -O0 -g

.PHONY: all test1 task1 clean

all: test1

#Task1
test1: task1

task1:
	$(CXX) $(CFLAGS) -o task1 task1.c
	./printProc 5

#Task2


clean:
	rm -f printProc
