#include <stdio.h>

int a = 1;

void f(int a) {
    if (a) {
        printf("Sim\n");
    } else {
        printf("Nao\n");
    }
}

int main() {
    f(a);
}

