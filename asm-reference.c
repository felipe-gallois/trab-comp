#include <stdio.h>

int a = 0;
float b = 0.0f;
char c = '0';

int main() {
    printf("Digite um int: ");
    scanf(" %d", &a);
    printf("%d\n", a);

    printf("Digite um float: ");
    scanf(" %f", &b);
    printf("%f\n", b);

    printf("Digite um char: ");
    scanf(" %c", &c);
    printf("%c\n", c);
}

