%{
    #include "tokens.h"

    static int running = 1;
%}

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

"<="        { return OPERATOR_LE; }
">="        { return OPERATOR_GE; }
"=="        { return OPERATOR_EQ; }
"!="        { return OPERATOR_DIF; }

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
