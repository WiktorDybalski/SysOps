CXX = gcc
CFLAGS = -Wall -Wextra -O0 -g

.PHONY: task1

all: task1 task2 calculate_integration

task1:
	$(CXX) $(CFLAGS) -o task1 task1.c
	./task1 0.0000000005, 40
	./task1 0.0000000005, 10
	./task1 0.0000000005, 4
	./task1 0.0000000005, 1

task2:
	$(CXX) $(CFLAGS) -o calculate_integration calculate_integration.c
	$(CXX) $(CFLAGS) -o task2 task2.c
clean:
	rm -f task1 task2 calculate_integration time_file.txt