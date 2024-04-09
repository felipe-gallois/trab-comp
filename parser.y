%{
#include <stdlib.h>
#include <stdio.h>

int yylex();
int yyerror();
int getLineNumber();
%}

%token KW_CHAR
%token KW_INT
%token KW_FLOAT
%token KW_BOOL

%token KW_IF
%token KW_ELSE
%token KW_WHILE
%token KW_READ
%token KW_PRINT
%token KW_RETURN

%token OPERATOR_LE
%token OPERATOR_GE
%token OPERATOR_EQ
%token OPERATOR_DIF

%token TK_IDENTIFIER

%token LIT_INT
%token LIT_CHAR
%token LIT_REAL
%token LIT_FALSE
%token LIT_TRUE
%token LIT_STRING

%token TOKEN_ERROR

%%

programa: TK_IDENTIFIER
        | LIT_INT
        ;

%%

int yyerror() {
    fprintf(stderr, "Syntax error at line %d\n", getLineNumber());
    exit(3);
}
