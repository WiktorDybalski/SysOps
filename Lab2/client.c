#include <stdio.h>
#include "collatz.c"

int main() {
    int result1 = test_collatz_convergence(10, 100);
    int result2 = test_collatz_convergence(11, 100);
    int result3 = test_collatz_convergence(12, 100);

    if (result1 == 7) {
        printf("%d\n", result1);
    }
    if (result2 == 15) {
        printf("%d\n", result2);
    }
    if (result3 == 10) {
        printf("%d\n", result3);
    }
}