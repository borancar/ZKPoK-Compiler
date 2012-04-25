.SUFFIXES: .cpp .o

CC = clang++
CFLAGS = -g -fPIC -Wall -lgmp -lgmpxx

.cpp.o:
	$(CC) $(CFLAGS) -c `llvm-config --cppflags` $<

all: LLVMHWSWBackend.so LLVMMontgomery.so

LLVMHWSWBackend.so : HWSWBackend.o
	$(CC) $(CFLAGS) -shared $< `llvm-config --ldflags` -o $@

LLVMMontgomery.so : MontgomeryDomain.o
	$(CC) $(CFLAGS) -shared $< `llvm-config --ldflags` -o $@

clean:
	rm -f *.o
	rm -f *~
