# building tests
# OPT=-g
OPT=-O2

RX=../src

RXDEPS=$(RX)/librx.a $(RX)/rx.h
CXXFLAGS=-std=c++0x -Wall $(OPT) -I$(RX)
LDLIBS=-L$(RX) -lrx

CXXFLAGS+=-DNO_POSIX

%.exe: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDLIBS)

TESTS=rx-testl.exe rx-matchl.exe

all: $(TESTS)

clean:
	del $(TESTS)

