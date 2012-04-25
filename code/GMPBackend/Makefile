.SUFFIXES: .cpp .o

CC = clang++
CFLAGS = -g -fPIC -Wall

.cpp.o:
	$(CC) $(CFLAGS) -c `llvm-config --cppflags` $<

all: LLVMGMPBackend.so

LLVMGMPBackend.so : GMPBackend.o
	$(CC) $(CFLAGS) -shared $< `llvm-config --ldflags` -lgmpxx -lgmp -o $@

clean:
	rm -f *.o
	rm -f *~
