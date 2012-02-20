.SUFFIXES: .cpp .o

.cpp.o:
	g++ -fPIC -Wall -c $<

all: libterm.so

libterm.so: ipterm.o
	g++ -shared $< -lipconfig -lfdl -lgmp -ldl -o $@

link:
	socat PTY,link=./my_term PTY,link=./out&

unlink:
	pkill socat

test: libterm.so
	gtkterm -p out&
	fdlsim testterm.fdl -1

clean:
	rm -f *.o
	rm -f *~
