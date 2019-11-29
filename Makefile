CC=gcc
PROJECTDIR=build/dbrainfuck.out
INSTALLDIR=/bin/dbrainfuck
CFLAGS= -g
INSTALLFLAGS=

default: src/dbrainfuck.c
	$(CC) -o $(PROJECTDIR) $^ $(CFLAGS)

install: src/dbrainfuck.c
	$(CC) -o $(INSTALLDIR) $^ $(INSTALLFLAGS)
	make clean

clean:
	rm -f *.o

run:
	make default
	./$(PROJECTDIR)

rc:
	make run clean
