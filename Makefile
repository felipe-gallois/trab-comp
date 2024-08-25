#
# UFRGS - Compiladores - Felipe Gallois - 2024/1
#
#

CC = gcc
CFLAGS = -Wall -g

SRC1 = y.tab.c lex.yy.c
SRC2 = hash.c ast.c semantic.c decompilation.c tac.c optimization.c asm.c main.c
SRC = $(SRC1) $(SRC2)

OBJS = $(SRC:.c=.o)

AUX1 = $(SRC1:.c=.h)

BIN = etapa7

INPUT = asm-test.txt
OUTPUT = output.txt
ASM = assembly.s

TEST_OUT = test-out
PRINT_OUT = test-print

$(BIN): $(OBJS)
	$(CC) -o $(BIN) $(OBJS)
	
lex.yy.c: scanner.l
	flex --header-file=lex.yy.h scanner.l 

y.tab.c: parser.y
	yacc -d parser.y

.PHONY: test
test:
	./$(BIN) $(INPUT) $(OUTPUT)
	$(CC) -o $(TEST_OUT) $(CFLAGS) $(ASM)
	./$(TEST_OUT) > $(PRINT_OUT)

.PHONY: clean
clean:
	rm $(OBJS) $(SRC1) $(AUX1) $(BIN) *.s *.out
