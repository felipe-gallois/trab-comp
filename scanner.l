%{
    #include "tokens.h"

    static int running = 1;
%}

%x SINGLE_COMMENT
%x MULTI_COMMENT

%%

"char"     { return KW_CHAR; }
"int"     { return KW_INT; }
"float"     { return KW_FLOAT; }

"code"     { return KW_CODE; }

"if"     { return KW_IF; }
"else"     { return KW_ELSE; }
"while"     { return KW_WHILE; }
"input"     { return KW_INPUT; }
"print"     { return KW_PRINT; }
"return"     { return KW_RETURN; }

"//"        { BEGIN(SINGLE_COMMENT); }
<SINGLE_COMMENT>.
<SINGLE_COMMENT>"\n"        { BEGIN(INITIAL); }

"///"        { BEGIN(MULTI_COMMENT); }
<MULTI_COMMENT>.
<MULTI_COMMENT>"\n"
<MULTI_COMMENT>"\\\\\\"       { BEGIN(INITIAL); }

[-,;()\[\]{}=+*/%<>&|~]     { return yytext[0]; }

"<="        { return OPERATOR_LE; }
">="        { return OPERATOR_GE; }
"=="        { return OPERATOR_EQ; }
"!="        { return OPERATOR_DIF; }

[0-9a-zA-Z\_]*[a-zA-Z\_][0-9a-zA-Z\_]*      { return TK_IDENTIFIER; } 

[ \t\n]

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
