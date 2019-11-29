CC=gcc
PROJECTDIR=build/dbrainfuck.out
INSTALLDIR=/bin/dbrainfuck
CFLAGS=
INSTALLFLAGS=

default: src/dbrainfuck.c
	mkdir $(PROJECTDIR)
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
