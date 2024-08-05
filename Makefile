#
# UFRGS - Compiladores - Felipe Gallois - 2024/1
#
#

CC = gcc
CFLAGS = -Wall -g

SRC1 = y.tab.c lex.yy.c
SRC2 = hash.c ast.c semantic.c decompilation.c tac.c main.c
SRC = $(SRC1) $(SRC2)

OBJS = $(SRC:.c=.o)

AUX1 = $(SRC1:.c=.h)

BIN = etapa6

$(BIN): $(OBJS)
	$(CC) -o $(BIN) $(OBJS)
	
lex.yy.c: scanner.l
	flex --header-file=lex.yy.h scanner.l 

y.tab.c: parser.y
	yacc -d parser.y

.PHONY: clean
clean:
	rm $(OBJS) $(SRC1) $(AUX1) $(BIN)
