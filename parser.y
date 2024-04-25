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
%type<ast_node> type
%type<ast_node> expr
%type<ast_node> arg
%type<ast_node> param
%type<ast_node> args_l
%type<ast_node> param_l
%type<ast_node> args_list
%type<ast_node> param_list
%type<ast_node> literal_list
%type<ast_node> empty_command
%type<ast_node> variable_attribution
%type<ast_node> vector_attribution
%type<ast_node> read_command
%type<ast_node> print_command
%type<ast_node> return_command
%type<ast_node> if_expr
%type<ast_node> while_expr
%type<ast_node> command
%type<ast_node> commands
%type<ast_node> command_block
%type<ast_node> function_declaration
%type<ast_node> vector_declaration
%type<ast_node> variable_declaration
%type<ast_node> declaration
%type<ast_node> declarations
%type<ast_node> program

%left '|'
%left '&'
%left OPERATOR_EQ OPERATOR_DIF
%nonassoc '>' OPERATOR_GE '<' OPERATOR_LE
%left '+' '-'
%left '*' '/'
%nonassoc '~'

%%

program: declarations       { $$ = ast_create(AST_PROGRAM, 0, $1, 0, 0, 0); ast_print($$, 0); }
       ;

declarations:                               { $$ = ast_create(AST_DECL_LIST, 0, 0, 0, 0, 0); }
            | declaration declarations      { $$ = ast_create(AST_DECL_LIST, 0, $1, $2, 0, 0); }
            ;

declaration: variable_declaration       { $$ = $1; }
           | vector_declaration     { $$ = $1; }
           | function_declaration       { $$ = $1; }
           ;

variable_declaration: type identifier':'literal';'      { $$ = ast_create(AST_VAR_DECL, 0, $1, $2, $4, 0); }
                    ;

vector_declaration: type identifier'['literal']'';'     { $$ = ast_create(AST_VEC_DECL, 0, $1, $2, $4, 0); }
                  | type identifier'['literal']'':' literal_list';'     { $$ = ast_create(AST_VEC_DECL, 0, $1, $2, $4, $7); }
                  ;

function_declaration: type identifier '('param_list')' command_block { $$ = ast_create(AST_FUNC_DECL, 0, $1, $2, $4, $6); }
                    ;

command_block: '{'commands'}'       { $$ = ast_create(AST_BLOCK, 0, $2, 0, 0, 0); }
             ;

commands:                       { $$ = 0; }
        | command commands      { $$ = ast_create(AST_CMD_LIST, 0, $1, $2, 0, 0); }
        ;

command: empty_command      { $$ = $1; }
       | variable_attribution       { $$ = $1; }
       | vector_attribution     { $$ = $1; }
       | read_command       { $$ = $1; }
       | print_command      { $$ = $1; }
       | return_command     { $$ = $1; }
       | if_expr        { $$ = $1; }
       | while_expr     { $$ = $1; }
       | command_block      { $$ = $1; }
       ;

variable_attribution: identifier '=' expr';'        { $$ = ast_create(AST_VAR_ATTRIB, 0, $1, $3, 0, 0); }
                    ;

vector_attribution: identifier'['expr']' '=' expr';'        { $$ = ast_create(AST_VEC_ATTRIB, 0, $1, $3, $6, 0); }
                  ;

read_command: KW_READ type expr';'      { $$ = ast_create(AST_READ, 0, $2, $3, 0, 0); }
            ;

print_command: KW_PRINT expr';'     { $$ = ast_create(AST_PRINT, 0, 0, $2, 0, 0); }
             | KW_PRINT type expr';'        { $$ = ast_create(AST_PRINT, 0, $2, $3, 0, 0); }
             ;

return_command: KW_RETURN expr';'       { $$ = ast_create(AST_RET, 0, $2, 0, 0, 0); }
              ;

if_expr: KW_IF '('expr')' command       { $$ = ast_create(AST_IF, 0, $3, $5, 0, 0); }
       | KW_IF '('expr')' command KW_ELSE command       { $$ = ast_create(AST_IF, 0, $3, $5, $7, 0); }
       ;

while_expr: KW_WHILE '('expr')' command     { $$ = ast_create(AST_WHILE, 0, $3, $5, 0, 0); }
          ;

expr: literal       { $$ = $1; }
    | identifier     { $$ = $1; }
    | identifier'['expr']'       { $$ = ast_create(AST_VEC_EXP, 0, $1, $3, 0, 0); }
    | identifier'('args_list')'      { $$ = ast_create(AST_FUNC_EXP, 0, $1, $3, 0, 0); }
    | '('expr')'        { $$ = $2; }
    | expr '+' expr     { $$ = ast_create(AST_SUM, 0, $1, $3, 0, 0); }
    | expr '-' expr     { $$ = ast_create(AST_SUB, 0, $1, $3, 0, 0); }
    | expr '*' expr     { $$ = ast_create(AST_MUL, 0, $1, $3, 0, 0); }
    | expr '/' expr     { $$ = ast_create(AST_DIV, 0, $1, $3, 0, 0); }
    | expr '<' expr     { $$ = ast_create(AST_LESS, 0, $1, $3, 0, 0); }
    | expr '>' expr     { $$ = ast_create(AST_GREATER, 0, $1, $3, 0, 0); }
    | expr OPERATOR_LE expr     { $$ = ast_create(AST_LE, 0, $1, $3, 0, 0); }
    | expr OPERATOR_GE expr     { $$ = ast_create(AST_GE, 0, $1, $3, 0, 0); }
    | expr OPERATOR_EQ expr     { $$ = ast_create(AST_EQ, 0, $1, $3, 0, 0); }
    | expr OPERATOR_DIF expr        { $$ = ast_create(AST_DIF, 0, $1, $3, 0, 0); }
    | expr '&' expr     { $$ = ast_create(AST_AND, 0, $1, $3, 0, 0); }
    | expr '|' expr     { $$ = ast_create(AST_OR, 0, $1, $3, 0, 0); }
    | '~' expr     { $$ = ast_create(AST_NOT, 0, $2, 0, 0, 0); }
    ;

literal_list: literal       { $$ = ast_create(AST_LIT_LIST, 0, $1, 0, 0, 0); }
            | literal literal_list      { $$ = ast_create(AST_LIT_LIST, 0, $1, $2, 0, 0); }
            ;

param_list:                     { $$ = 0; } 
          | param param_l       { $$ = ast_create(AST_PARAM_LIST, 0, $1, $2, 0, 0); }
          ;

args_list:                  { $$ = 0; }
         | arg args_l       { $$ = ast_create(AST_ARGS_LIST, 0, $1, $2, 0, 0); }
         ;

param_l:                        { $$ = 0; }
       | ','param param_l       { $$ = ast_create(AST_PARAM_LIST, 0, $2, $3, 0, 0); }
       ;

args_l:                     { $$ = 0; }
      | ','arg args_l       { $$ = ast_create(AST_ARGS_LIST, 0, $2, $3, 0, 0); }
      ;

param: type identifier      { $$ = ast_create(AST_PARAM, 0, $1, $2, 0, 0); }
     ;

arg: expr       { $$ = $1; }
   ;

empty_command: ';'      { $$ = ast_create(AST_EMPTY, 0, 0, 0, 0, 0); }
             ;

type: KW_CHAR       { $$ = ast_create(AST_CHAR, 0, 0, 0, 0, 0); }
    | KW_INT        { $$ = ast_create(AST_INT, 0, 0, 0, 0, 0); }
    | KW_FLOAT      { $$ = ast_create(AST_FLOAT, 0, 0, 0, 0, 0); }
    | KW_BOOL       { $$ = ast_create(AST_BOOL, 0, 0, 0, 0, 0); }
    ;

identifier: TK_IDENTIFIER       { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); }
          ;

literal: LIT_INT        { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); }
       | LIT_CHAR       { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); }
       | LIT_REAL       { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); }
       | LIT_FALSE      { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); }
       | LIT_TRUE       { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); }
       | LIT_STRING     { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); }
       ;

%%

int yyerror() {
    fprintf(stderr, "Syntax error at line %d\n", getLineNumber());
    exit(3);
}
