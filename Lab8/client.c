#include <stdlib.h>
#include <stdio.h>

void generate_random_string(char* buffer, int n) {
    for (int i = 0; i < n; i++) {
        buffer[i] = 'a' + rand() % 26;
    }
    buffer[n] = '\n';
}

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Usage: %s <number_of_users>\n", argv[0]);
        return -1;
    }
    int number_of_users = atoi(argv[1]);


}