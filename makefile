CC=gcc
CFLAGS=-Wall -O0

all: myblas.o test_iaxpy

clean:
	rm -f *.o *.out

myblas.o:	myblas.c 
	${CC} -c $*.c ${CFLAGS}

test_iaxpy:	myblas.o test_iaxpy.o
	${CC} $@.o myblas.o -o $@.out ${CFLAGS}

test_iaxpy.o:	test_iaxpy.c myblas.h
	${CC} -c $*.c ${CFLAGS}
