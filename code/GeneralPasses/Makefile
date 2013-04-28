.SUFFIXES: .cpp .o

CC = clang++
CFLAGS = -g -fPIC -Wall

.cpp.o:
	$(CC) $(CFLAGS) -c `llvm-config --cppflags` $<

all: LLVMOptStoreLoad.so LLVMMontgomery.so

MontgomeryDomain.o: MontgomeryDomain.h

LLVMMontgomery.so: MontgomeryDomain.o
	$(CC) $(CFLAGS) -lgmpxx -shared $< `llvm-config --ldflags` -o $@

LLVMOptStoreLoad.so: OptStoreLoad.o
	$(CC) $(CFLAGS) -shared $< `llvm-config --ldflags` -o $@

clean:
	rm -f *.o
	rm -f *~
