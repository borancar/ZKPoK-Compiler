#Target directories
BIN_DIR	= .

#programs to install
PROGS	= prover verifier

#object files of specific programs
C_OBJS  = includes/math.o includes/mexp.o includes/comm.o includes/comm-socket.o includes/comm-term.o includes/galois.o includes/config.o includes/reporting.c
P_OBJS	= $(C_OBJS) prover.o
V_OBJS	= $(C_OBJS) verifier.o

#c compiling parameters
CFLAGS      = -DCOMM=COMM_TERM
CCFLAGS     = -Wall -g -lm -lgmp -lrt   $(CFLAGS)
CC		    = gcc
LIBS        = -lgmp -lm -lrt

#compiling the c-files
.c.o:
	$(CC) -c $(CCFLAGS) -o $*.o $<


#standard make rule
all:	$(PROGS)


#object linking to executables
verifier: $(V_OBJS)	
	$(CC) $(V_OBJS) $(LIBS) -o $@ 

prover:  $(P_OBJS)	
	$(CC) $(P_OBJS) $(LIBS) -o $@





#installing the progs.
install: $(PROGS)
	install $(PROGS) $(BIN_DIR)


#cleaning the local object files and the local and installed executables
clean:
	rm -f *.o *~ 
	rm -f includes/*.o
	rm -f test
	@for i in $(PROGS); do \
	  rm -f $$i; \
	done
	rm TAGS

link:
	socat PTY,link=term_prover PTY,link=term_verifier &

unlink:
	pkill socat
