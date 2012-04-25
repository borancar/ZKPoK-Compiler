.SUFFIXES: .cpp .o

.cpp.o:
	g++ -fPIC -Wall -c $<

all: libterm.so libtransceiver.so

libterm.so: ipterm.o
	g++ -g -fPIC -shared $< -lipconfig -lfdl -lgmp -ldl -o $@

libtransceiver.so: iptransceiver.cpp
	g++ -g -fPIC -shared $< -lipconfig -lfdl -lgmp -ldl -o $@

link:
	socat PTY,link=./my_term PTY,link=./out&

unlink:
	pkill socat

test: libterm.so
	fdlsim testtransceiver.fdl -1

clean:
	rm -f *.o
	rm -f *~
