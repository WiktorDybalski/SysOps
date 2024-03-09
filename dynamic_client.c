#include <stdio.h>
#include <dlfcn.h>

int main() {
    void *uchwyt = dlopen("./libcollatz.so", RTLD_LAZY);
    if(!uchwyt) {
        printf("Blad otwierania biblioteki\n");
        return 0;
    }
    void (*f2)(void);
    f2 = dlsym(uchwyt, "wypisz");
    if(dlerror() != 0) {
        printf("blad fun 2\n");
        return 0;
    }
    f2();
    dlclose(uchwyt);
    return 0;
}