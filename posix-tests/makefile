# building tests
#OPT=-g
OPT=-O2

RX=../src

RXDEPS=$(RX)/librx.a $(RX)/rx.h
CXXFLAGS=-std=c++0x -Wall $(OPT) -I$(RX)
LDLIBS=-L$(RX) -lrx

TESTS=rx-test rx-match

all: $(TESTS)

clean:
	rm $(TESTS)


