# Compiler used is gcc
CXX = gcc
# Compiler flags: -Wall (enable all warnings), -Wextra (enable extra warnings), -O0 (disable optimization), -g (include debugging information)
CFLAGS = -Wall -Wextra -O0 -g

.PHONY: all test1 test2 clean

all: test1 test2

# Zad 1
test1: test_bytes test_blocks

test_bytes:reverse_bytes
	./reverse bin_file tmp1;
	#./reverse tmp1 tmp2;
	diff -s tmp2 bin_file

test_blocks:reverse_blocks
	./reverse bin_file tmp1;
	./reverse tmp1 tmp2;
	diff -s tmp2 bin_file

reverse_bytes:
	$(CXX) $(CFLAGS) -o reverse reverse.c -D BYTES

reverse_blocks:
	$(CXX) $(CFLAGS) -o reverse reverse.c

# Zad 2
test2:
	$(CXX) $(CFLAGS) -o find find.c
	./find
clean:
	rm -f reverse tmp1 tmp2 find pomiar_zad_2.txt

