CXX = gcc
CFLAGS = -Wall -Wextra -O0 -g

.PHONY: task1

all: task1 task2 calculate_integration

task1:
	$(CXX) $(CFLAGS) -o task1 task1.c
	./task1 0.000001, 5000
	./task1 0.000001, 10000
	./task1 0.000001, 15000
	./task1 0.00001, 20000
	./task1 0.0001, 20000

task2:
	$(CXX) $(CFLAGS) -o calculate_integration calculate_integration.c
	$(CXX) $(CFLAGS) -o task2 task2.c
clean:
	rm -f task1 task2 calculate_integration time_file.txt