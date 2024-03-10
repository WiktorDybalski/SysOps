CXX = gcc
CFLAGS = -Wall -Wextra -g

.PHONY: all static_lib shared_lib client_static client_shared client_dynamic clean

all: static_lib shared_lib client_static client_shared client_dynamic
#utworzenie biblioteki statycznej z rozszerzeniem .a
static_lib:
	gcc -c collatz.c
	ar rcs libcollatz.a collatz.o
#utworzenie bilioteki dynamicznej z rozszerzeniem .so
shared_lib:
	gcc -fPIC -c collatz.c
	gcc -shared collatz.o -o libcollatz.so
#utworzenie klienta z biblioteką statyczną
client_static: static_lib
	gcc client.c libcollatz.a -o client_static
#utworzenie klienta z biblioteką współdzieloną
client_shared: shared_lib
	gcc client.c -L. -libcollatz -Wl, -rpath=. -o client_shared
#utworzenie klienta z biblioteką dynamiczną
client_dynamic:
	gcc client_dynamic.c -o client_dynamic -ldl
clean:
	rm -f *.a *.o *.so client_static client_shared client_dynamic
