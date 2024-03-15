# Compiler used is gcc
CXX = gcc
# Compiler flags: -Wall (enable all warnings), -Wextra (enable extra warnings), -O0 (disable optimization), -g (include debugging information)
CFLAGS = -Wall -Wextra -O0 -g

.PHONY: all main clean

all: create_files main

find_files:
	$(CXX) $(CFLAGS) -o find find.c
	./find
main: main
	$(CXX) $(CFLAGS) -o main main.c
	./main

create_files: create_reversed_file3 create_reversed_file4

create_reversed_file3:
	echo "" > reversed_file3
create_reversed_file4:
	echo "" > reversed_file4

clean:
	rm -f main reversed_file3 reversed_file4
