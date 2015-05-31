# Rx library
OPT=-g
# OPT=-O2

CFLAGS=-std=c99 -c $(OPT) -Wall
CXXFLAGS=-std=c++0x -c $(OPT) -Wall

librx.a: rx.o lua-str.o
	ar rcu librx.a rx.o lua-str.o && ranlib librx.a

clean:
	rm *.a *.o   