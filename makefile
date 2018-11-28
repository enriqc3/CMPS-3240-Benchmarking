CC=gcc
CFLAGS=-Wall -O0

all: myblas.o test_program

clean:
	rm -f *.o *.out

myblas.o:	myblas.c 
	${CC} -c $*.c ${CFLAGS}

test_program:	myblas.o test_program.o
	${CC} $@.o myblas.o -o $@ ${CFLAGS}

test_program.o:	test_program.c myblas.h
	${CC} -c $*.c ${CFLAGS}
