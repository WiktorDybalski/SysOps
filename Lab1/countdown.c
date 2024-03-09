#include <stdio.h>
#include <unistd.h>

int main() {
    for(int i = 10; i > 0; i--) {
        printf("%d\n", i);
        sleep(1);
    }
    printf("Hello, World!\n");
    printf("Hello, World!\n");
    printf("Hello, World!\n");
    return 0;
}
