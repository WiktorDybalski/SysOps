#include <stdio.h>
#include <dlfcn.h>

int main() {
    void *dlhandle = dlopen("./libcollatz.so", RTLD_LAZY);
    if (!dlhandle) {
        printf("Library Opening Error!\n");
        return 0;
    }
    int (*collatz_conjecture)(int input);
    int (*test_collatz_convergence)(int input, int max_iter);

    collatz_conjecture = dlsym(dlhandle, "collatz_conjecture");
    test_collatz_convergence = dlsym(dlhandle, "test_collatz_convergence");

    printf("%d\n", collatz_conjecture(10));

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
    dlclose(dlhandle);
    return 0;
}