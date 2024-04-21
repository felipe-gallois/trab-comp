#
# UFRGS - Compiladores - Felipe Gallois - 2024/1
#
# Este arquivo define as receitas relacionadas à compilação do lexer
#

CC = gcc
CFLAGS = -Wall

SRC1 = y.tab.c lex.yy.c
SRC2 = hash.c ast.c main.c
SRC = $(SRC1) $(SRC2)

OBJS = $(SRC:.c=.o)

AUX1 = $(SRC1:.c=.h)

BIN = etapa3

$(BIN): $(OBJS)
	$(CC) -o $(BIN) $(OBJS)
	
lex.yy.c: scanner.l
	flex --header-file=lex.yy.h scanner.l 

y.tab.c: parser.y
	yacc -d parser.y

.PHONY: clean
clean:
	rm $(OBJS) $(SRC1) $(AUX1) $(BIN)
