#include <stdio.h>

long w = 7;
float x = 8.0f;
char y = 'i';

int main() {
    // Impressão de literais
    printf("%ld", 1l + 2l);
    printf("%f", 2.0f + 3.0f);
    printf("%c", 'c');
    printf("def");

    // Soma
    w += 2l;

    // Impressão de variáveis
    printf("%ld", w);
    printf("%f", x);
    printf("%c", y);
}

long a() { return 0; }
float b() { return 0.0f; }
char c() { return '\0'; }
