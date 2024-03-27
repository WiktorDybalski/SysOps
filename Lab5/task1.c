#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>


void signal_handler() {
    printf("Signal handled!");
}
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Wrong number of arguments: Failed\n");
        return -1;
    }

    if (strcmp(argv[1], "ignore") == 0) {
        if (signal(SIGUSR1, SIG_IGN) == SIG_ERR) {
            perror("Ignore Signal Error");
            exit(EXIT_FAILURE);
        }
    } else if (strcmp(argv[1], "handler") == 0) {
        if(signal(SIGUSR1, signal_handler) == SIG_ERR) {
            perror("Handler Signal Error");
            exit(EXIT_FAILURE);
        }
    } else if (strcmp(argv[1], "mask") == 0) {
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);
        if (sigprocmask(SIG_BLOCK, &set, NULL) < 0) {
            perror("Maks signal error");
            exit(EXIT_FAILURE);
        }
    }
}