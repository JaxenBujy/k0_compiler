all: scanner

scanner: main.o lex.yy.o
	gcc -g -Wall -o scanner main.o lex.yy.o
lex.yy.c: k0lex.l
	flex k0lex.l
lex.yy.o: lex.yy.c
	gcc -g -Wall -c lex.yy.c
main.o: main.c
	gcc -g -Wall -c main.c
clean:
	rm -f *.o lex.yy.c scanner
