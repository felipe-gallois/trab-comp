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

%left '|'
%left '&'
%left OPERATOR_EQ OPERATOR_DIF
%left '>' OPERATOR_GE '<' OPERATOR_LE
%left '+' '-'
%left '*' '/'
%left '~'

%%

program: commands
       ;

command_block: '{'commands'}'
             ;

commands: command commands
        |
        ;

command: empty_command
       | variable_declaration
       | vector_declaration
       | variable_attribution
       | vector_attribution
       | read_command
       | print_command
       | return_command
       | if_expr
       | while_expr
       | function_declaration
       | command_block
       ;

variable_declaration: type TK_IDENTIFIER':'literal';'
                    ;

vector_declaration: type TK_IDENTIFIER'['LIT_INT']'';'
                  | type TK_IDENTIFIER'['LIT_INT']'':' literal_list';'
                  ;

variable_attribution: TK_IDENTIFIER '=' expr';'
                    ;

vector_attribution: TK_IDENTIFIER'['expr']' '=' expr';'
                  ;

read_command: KW_READ type expr';'
            ;

print_command: KW_PRINT expr';'
             | KW_PRINT type expr';'
             ;

return_command: KW_RETURN expr';'
              ;

if_expr: KW_IF '('expr')' command
       | KW_IF '('expr')' command KW_ELSE command
       ;

while_expr: KW_WHILE '('expr')' command
          ;

function_declaration: type TK_IDENTIFIER '('param_list')' command_block
                    ;

expr: literal
    | TK_IDENTIFIER
    | TK_IDENTIFIER'['expr']'
    | TK_IDENTIFIER'('args_list')'
    | '('expr')'
    | expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | expr '<' expr
    | expr '>' expr
    | expr OPERATOR_LE expr
    | expr OPERATOR_GE expr
    | expr OPERATOR_EQ expr
    | expr OPERATOR_DIF expr
    | expr '&' expr
    | expr '|' expr
    | expr '~' expr
    ;

literal_list: literal
            | literal literal_list
            ;

param_list: 
          | param param_l
          ;

args_list: arg args_l
         ;

param_l:
       | ','param param_l
       ;

args_l:
      | ','arg args_l
      ;

param: type TK_IDENTIFIER
     ;

arg: expr
   ;

empty_command: ';'
             ;

type: KW_CHAR
    | KW_INT
    | KW_FLOAT
    | KW_BOOL
    ;

literal: LIT_INT
       | LIT_CHAR
       | LIT_REAL
       | LIT_FALSE
       | LIT_TRUE
       | LIT_STRING
       ;

%%

int yyerror() {
    fprintf(stderr, "Syntax error at line %d\n", getLineNumber());
    exit(3);
}
