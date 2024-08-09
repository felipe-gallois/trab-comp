#include <stdio.h>

int a = 365;
int b = 45;

char c = 'a';
char d = 'b';

int main() {
    printf("%d\n", (int) (a + b));
    printf("%d\n", (int) (a + d));
    printf("%d\n", (int) (c + b));
    printf("%d\n", (int) (c + d));
}

