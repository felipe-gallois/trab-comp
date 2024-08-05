#include <stdio.h>

long w = 7;
float x = 8.0f;
char y = 'i';

int main() {
    // Impressão de literais
    printf("%ld", 1l);
    printf("%f", 2.0f);
    printf("%c", 'c');
    printf("def");

    // Impressão de variáveis
    printf("%ld", w);
    printf("%f", x);
    printf("%c", y);
}
