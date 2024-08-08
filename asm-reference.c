#include <stdio.h>

int intvar[3] = { 0, 0, 0 };
float floatvar[3] = { 1.0f, 1.0f, 1.0f };
char charvar[3] = { 'a', 'a', 'a' };

int a = 0;
int b = 1;
int c = 2;

int d = 3;
float e = 4.0f;
char f = 'c';

float res = 0.0f;

int main() {
    intvar[a] = d;
    floatvar[b] = e;
    charvar[c] = f;

    res = floatvar[b];

    printf("%d\n", intvar[a]);
    printf("%f\n", res);
    printf("%c\n", charvar[c]);
}

