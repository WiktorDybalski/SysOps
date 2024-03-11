# Compiler used is gcc
CXX = gcc

# C flags common in all build configurations
# disable all optimizations and add debug information in debug build
cflags.debug := -g -O0
# enable O2 optimization in release mode
cflags.release := -O2

# Compiler flags: -Wall (enable all warnings), -Wextra (enable extra warnings), -g (include debugging information)
CFLAGS = -Wall -Wextra -g

.PHONY: all static_lib shared_lib client_static client_shared client_dynamic clean

all: static_lib shared_lib client_static client_shared client_dynamic
# Creating static library with .a extension
static_lib:
	gcc -c collatz.c
	ar rcs libcollatz.a collatz.o
# Creating dynamic library with .a extension
shared_lib:
	gcc -fPIC -c collatz.c
	gcc -shared collatz.o -o libcollatz.so

# Creating client with static library
client_static: static_lib
	gcc client.c libcollatz.a -o client_static
# Creating client with shared library
client_shared: shared_lib
	gcc client.c -L. -lcollatz -Wl,-rpath=. -o client_shared
# Creating client with dynamic library
client_dynamic:
	gcc client.c -o client_dynamic -ldl -D DYNAMIC_DLOPEN

# Test clients
test_static: client_static
	./client_static
test_shared: client_shared
	./client_shared
test_dynamic: client_dynamic
	./client_dynamic

# Generate objdump for static client
objdump_static: client_static
	objdump -d client_static > client_static_objdump.txt

# Generate objdump for shared client
objdump_shared: client_shared
	objdump -d client_shared > client_shared_objdump.txt

# Generate objdump for dynamic client
objdump_dynamic: client_dynamic
	objdump -d client_dynamic > client_dynamic_objdump.txt

clean:
	rm -f *.a *.o *.so client_static client_shared client_dynamic client_static_objdump.txt client_shared_objdump.txt client_dynamic_objdump.txt
