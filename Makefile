#
# UFRGS - Compiladores - Felipe Gallois - 2024/1 - Etapa 1
#
# Este arquivo define as receitas relacionadas à compilação do lexer da etapa 1
# do trabalho. 
#

etapa1: lex.yy.o hash.o main.o
	gcc -o etapa1 lex.yy.o hash.o main.o

main.o: main.c
	gcc -c main.c

hash.o: hash.c
	gcc -c hash.c

lex.yy.o: lex.yy.c
	gcc -c lex.yy.c
	
lex.yy.c: scanner.l
	flex --header-file=lex.yy.h scanner.l 

clean:
	rm *.o lex.yy.c lex.yy.h etapa1
