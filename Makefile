#
# UFRGS - Compiladores - Felipe Gallois - 2024/1 - Etapa 1
#
# Este arquivo define as receitas relacionadas à compilação do lexer da etapa 1
# do trabalho. 
#

etapa2: y.tab.o lex.yy.o hash.o main.o
	gcc -o etapa2 y.tab.o lex.yy.o hash.o main.o

main.o: main.c
	gcc -c -Wall main.c

hash.o: hash.c
	gcc -c -Wall hash.c

lex.yy.o: lex.yy.c
	gcc -c -Wall lex.yy.c

y.tab.o: y.tab.c
	gcc -c -Wall y.tab.c
	
lex.yy.c: scanner.l
	flex --header-file=lex.yy.h scanner.l 

y.tab.c: parser.y
	yacc -d parser.y

clean:
	rm *.o y.tab.c lex.yy.c y.tab.h lex.yy.h etapa2
