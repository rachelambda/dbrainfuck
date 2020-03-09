CC=c99
PROJECTDIR=dbrainfuck
INSTALLDIR=/bin/dbrainfuck
debug: CFLAGS=-g
INSTALLFLAGS=

default: src/dbrainfuck.c
	$(CC) -o $(PROJECTDIR) $^ $(CFLAGS)

debug: default

install: default
	cp $(PROJECTDIR) $(INSTALLDIR)

clean:
	rm -f *.o

run: default
	./$(PROJECTDIR)
