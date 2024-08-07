#include <stdio.h>

int w = 7;
float x = 8.0f;
char y = 'i';

int main() {
    // Impressão de literais
    printf("%d", 1 + 2);
    printf("%f", 2.0f + 3.0f);
    printf("%c", 'c');
    printf("def");

    // Soma
    w += 2;

    // Impressão de variáveis
    printf("%d", w);
    printf("%f", x);
    printf("%c", y);
}

int a() { return 0; }
float b() { return 0.0f; }
char c() { return '\0'; }
