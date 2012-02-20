CC = clang++

pil: pil.o codegen.o pilLexer.o pilParser.o
	${CC} -g -o pil pil.o codegen.o pilLexer.o pilParser.o -lantlr3c `llvm-config --ldflags --libs core jit native linker`

pilParser.h: pil.g
	antlr3 $<
	mv pilParser.c pilParser.cpp
	mv pilLexer.c pilLexer.cpp

pilLexer.h: pil.g
	antlr3 $<
	mv pilLexer.c pilLexer.cpp
	mv pilParser.c pilParser.cpp

pilParser.cpp: pil.g
	antlr3 $<
	mv pilParser.c pilParser.cpp
	mv pilLexer.c pilLexer.cpp

pilLexer.cpp: pil.g
	antlr3 $<
	mv pilLexer.c pilLexer.cpp
	mv pilParser.c pilParser.cpp

codegen.h: codegen.g
	antlr3 $<
	mv codegen.c codegen.cpp

codegen.cpp: codegen.g
	antlr3 $<
	mv codegen.c codegen.cpp

pil.o: pilParser.h pil.cpp
	${CC} -g -c `llvm-config --cppflags` pil.cpp

codegen.o: pilParser.h pilLexer.h GroupType.h codegen.cpp
	${CC} -g -c `llvm-config --cppflags` codegen.cpp

pilParser.o: pilParser.h pilLexer.h pilParser.cpp
	${CC} -g -c pilParser.cpp

pilLexer.o: pilLexer.h pilLexer.cpp
	${CC} -g -c pilLexer.cpp

clean:
	rm -f *~
	rm -f *.o
	rm -f pilLexer.*
	rm -f pilParser.*
	rm -f codegen.cpp
