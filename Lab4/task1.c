#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("Wrong amount of arguments");
        return -1;
    }
    int number_of_processes = atoi(argv[1]);
    for (int i = 0; i < number_of_processes; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork error");
            return 1;
        } else if (pid == 0) {
            printf("\nParent PID: %d, child PID %d\n", getppid(), getpid());
            exit(0);
        }
    }

    for (int i = 0; i < number_of_processes; i++) {
        wait(NULL);
    }
    printf("%s\n", argv[1]);
    return 0;
}