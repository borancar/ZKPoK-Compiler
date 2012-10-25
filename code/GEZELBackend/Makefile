.SUFFIXES: .cpp .o

CC = clang++
CFLAGS = -g -fPIC -Wall

.cpp.o:
	$(CC) $(CFLAGS) -c `llvm-config --cppflags` $<

all: LLVMGEZELBackend.so

GEZELWriter.o: GEZELWriter.h

GEZELTargetMachine.o: GEZELTargetMachine.h

LLVMGEZELBackend.so : GEZELWriter.o GEZELTargetMachine.o
	$(CC) $(CFLAGS) -shared $+ `llvm-config --ldflags` -o $@

clean:
	rm -f *.o
	rm -f *~
