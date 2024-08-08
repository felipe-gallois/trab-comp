#include <stdio.h>

int a = 0;
float b = 0.0f;
char c = 'a';

int main() {
    scanf("%d", &a);

    scanf("%f", &b);

    while (getchar() != '\n');
    scanf("%c", &c);

    printf("%d\n", a);
    printf("%f\n", b);
    printf("%c\n", c);
}

