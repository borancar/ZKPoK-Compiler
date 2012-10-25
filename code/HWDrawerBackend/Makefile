.SUFFIXES: .cpp .o

CC = clang++
CFLAGS = -g -fPIC -Wall

.cpp.o:
	$(CC) $(CFLAGS) -c `llvm-config --cppflags` $<

all: LLVMHWDrawerBackend.so

HWDrawerWriter.o: HWDrawerWriter.h

HWDrawerTargetMachine.o: HWDrawerTargetMachine.h

LLVMHWDrawerBackend.so : HWDrawerWriter.o HWDrawerTargetMachine.o
	$(CC) $(CFLAGS) -shared $+ `llvm-config --ldflags` -o $@

clean:
	rm -f *.o
	rm -f *~
