%{
// UFRGS - Compiladores - Felipe Gallois - 2024/1

#include <stdlib.h>
#include <stdio.h>

#include "hash.h"
#include "ast.h"

int yylex();
int yyerror();
int getLineNumber();
%}

%union {
HashEntry *hash_entry;
AstNode *ast_node;
}

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

%token<hash_entry> TK_IDENTIFIER

%token<hash_entry> LIT_INT
%token<hash_entry> LIT_CHAR
%token<hash_entry> LIT_REAL
%token<hash_entry> LIT_FALSE
%token<hash_entry> LIT_TRUE
%token<hash_entry> LIT_STRING

%token TOKEN_ERROR

%type<ast_node> identifier
%type<ast_node> literal
%type<ast_node> expr

%left '|'
%left '&'
%left OPERATOR_EQ OPERATOR_DIF
%nonassoc '>' OPERATOR_GE '<' OPERATOR_LE
%left '+' '-'
%left '*' '/'
%nonassoc '~'

%%

program: declarations
       ;

declarations:
            | declaration declarations
            ;

declaration: variable_declaration
           | vector_declaration
           | function_declaration
           ;

variable_declaration: type identifier':'literal';'
                    ;

vector_declaration: type identifier'['literal']'';'
                  | type identifier'['literal']'':' literal_list';'
                  ;

function_declaration: type identifier '('param_list')' command_block
                    ;

command_block: '{'commands'}'
             ;

commands:
        | command commands
        ;

command: empty_command
       | variable_attribution
       | vector_attribution
       | read_command
       | print_command
       | return_command
       | if_expr
       | while_expr
       | command_block
       ;

variable_attribution: identifier '=' expr';'
                    ;

vector_attribution: identifier'['expr']' '=' expr';'
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

expr: literal       { $$ = $1; }
    | identifier     { $$ = $1; }
    | identifier'['expr']'       { $$ = 0; }
    | identifier'('args_list')'      { $$ = 0; }
    | '('expr')'        { $$ = 0; }
    | expr '+' expr     { $$ = 0; }
    | expr '-' expr     { $$ = 0; }
    | expr '*' expr     { $$ = 0; }
    | expr '/' expr     { $$ = 0; }
    | expr '<' expr     { $$ = 0; }
    | expr '>' expr     { $$ = 0; }
    | expr OPERATOR_LE expr     { $$ = 0; }
    | expr OPERATOR_GE expr     { $$ = 0; }
    | expr OPERATOR_EQ expr     { $$ = 0; }
    | expr OPERATOR_DIF expr        { $$ = 0; }
    | expr '&' expr     { $$ = 0; }
    | expr '|' expr     { $$ = 0; }
    | expr '~' expr     { $$ = 0; }
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

param: type identifier
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

identifier: TK_IDENTIFIER       { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); ast_print($$, 0); }
          ;

literal: LIT_INT        { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); ast_print($$, 0); }
       | LIT_CHAR       { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); ast_print($$, 0); }
       | LIT_REAL       { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); ast_print($$, 0); }
       | LIT_FALSE      { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); ast_print($$, 0); }
       | LIT_TRUE       { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); ast_print($$, 0); }
       | LIT_STRING     { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); ast_print($$, 0); }
       ;

%%

int yyerror() {
    fprintf(stderr, "Syntax error at line %d\n", getLineNumber());
    exit(3);
}
