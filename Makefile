scanner: main.c
	gcc -o comp main.c scanner.c scanner.h parser.c parser.h statSem.c statSem.h
clean:
	-rm comp
