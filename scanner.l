%{
    #include "tokens.h"

    static int running = 1;
    static int line_number = 0;
%}

%x SINGLE_COMMENT
%x MULTI_COMMENT

%%

"bool"      { return KW_BOOL; }
"char"     { return KW_CHAR; }
"int"     { return KW_INT; }
"float"     { return KW_FLOAT; }

"if"     { return KW_IF; }
"else"     { return KW_ELSE; }
"while"     { return KW_WHILE; }
"read"     { return KW_READ; }
"print"     { return KW_PRINT; }
"return"     { return KW_RETURN; }

"false"     { return LIT_FALSE; }
"true"      { return LIT_TRUE; }

"//"        { BEGIN(SINGLE_COMMENT); }
<SINGLE_COMMENT>.
<SINGLE_COMMENT>"\n"        { ++line_number; BEGIN(INITIAL); }

"/*"        { BEGIN(MULTI_COMMENT); }
<MULTI_COMMENT>.
<MULTI_COMMENT>"\n"     { ++line_number; }
<MULTI_COMMENT>"*/"       { BEGIN(INITIAL); }

[-,;:()\[\]{}=+*/%<>&|~]     { return yytext[0]; }

"<="        { return OPERATOR_LE; }
">="        { return OPERATOR_GE; }
"=="        { return OPERATOR_EQ; }
"!="        { return OPERATOR_DIF; }

[0-9]+      { return LIT_INT; }

[0-9]+"."[0-9]+     { return LIT_REAL; }
[0-9]+"."       { return LIT_REAL; }
"."[0-9]+     { return LIT_REAL; }

"\'"."\'"       { return LIT_CHAR; } 

"\""[^\n"]*"\""     { return LIT_STRING; }

[a-zA-Z\_][0-9a-zA-Z\_]*      { return TK_IDENTIFIER; } 

[ \t]

"\n"        { ++line_number; }

.       { return TOKEN_ERROR; }

%%

int yywrap() {
   running = 0;
   return 1;
}

int isRunning() {
   return running; 
}

void initMe() {
   return;
}

int getLineNumber() {
    return line_number;
}
