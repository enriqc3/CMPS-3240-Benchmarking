CC=gcc
CFLAGS=-Wall -O0

all: myblas.o

clean:
	rm -f *.o *.out

myblas.o:	myblas.c 
	${CC} -c ${CFLAGS} $*.c