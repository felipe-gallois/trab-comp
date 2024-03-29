#include <stdio.h>
#include <stdlib.h>

//lex.yy.h
int yylex();
extern char *yytext;
extern FILE *yyin;


int isRunning(void);
void initMe(void);
int getLineNumber(void);

void print_hash();

int main(int argc, char** argv)
  {
  FILE *gold = 0;
  int token = 0;
  int answar = 0;
  int nota = 0;
  int i=1;
  int line_number = 0;

  if (argc < 3)
    {
    printf("call: ./etapa1 input.txt output.txt \n");
    exit(1);
    }
  if (0==(yyin = fopen(argv[1],"r")))
    {
    printf("Cannot open file %s... \n",argv[1]);
    exit(1);
    }
  if (0==(gold = fopen(argv[2],"r")))
    {
    printf("Cannot open file %s... \n",argv[2]);
    exit(1);
    }
  initMe();
  while (isRunning())
    {
    token = yylex();

    while (getLineNumber() > line_number) {
        fprintf(stderr, "LINE BREAK\n");
        line_number++;
    }
    
    if (!isRunning())
      break;
    fscanf(gold,"%d",&answar);
    if (token == answar)
      {
      fprintf(stderr,"%d=ok(%s)  ",i,yytext  );
      ++nota;
      }
    else
      fprintf(stderr,"\n%d=ERROR(%s,%d,%d) ",i,yytext,token,answar );
    fprintf(stderr, "\n");
    ++i;
    }
  printf("LINE COUNTER %d\n",line_number);
  printf("NOTA %d\n\n",nota);  
  fprintf(stderr,"NOTA %d\n\n",nota);  
  print_hash();
  }

