.SUFFIXES: .cpp .o

CC = clang++
CFLAGS = -g -fPIC -Wall

.cpp.o:
	$(CC) $(CFLAGS) -c `llvm-config --cppflags` $<

all: LLVMGMPBackend.so

GMPWriter.o: GMPWriter.h

GMPTargetMachine.o: GMPTargetMachine.h

LLVMGMPBackend.so : GMPWriter.o GMPTargetMachine.o
	$(CC) $(CFLAGS) -shared $+ `llvm-config --ldflags` -o $@

clean:
	rm -f *.o
	rm -f *~
