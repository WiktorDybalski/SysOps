#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

void signal_handler(int signum) {
    printf("Signal handled with number: %d\n", signum);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [none|ignore|handler|mask]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (strcmp(argv[1], "none") == 0) {
        signal(SIGUSR1, SIG_DFL);
        printf("Running with no special handling for SIGUSR1!\n");
        raise(SIGUSR1);
    } else if (strcmp(argv[1], "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);
        printf("Signal ignored for SIGUSR1!\n");
        raise(SIGUSR1);
    } else if (strcmp(argv[1], "handler") == 0) {
        signal(SIGUSR1, signal_handler);
        raise(SIGUSR1);
    } else if (strcmp(argv[1], "mask") == 0) {
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);
        sigprocmask(SIG_BLOCK, &set, NULL);

        raise(SIGUSR1);

        sigset_t pending;
        sigpending(&pending);
        if (sigismember(&pending, SIGUSR1)) {
            printf("SIGUSR1 is pending\n");
        } else {
            printf("SIGUSR1 is not pending\n");
        }
    } else {
        fprintf(stderr, "Invalid argument: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    return 0;
}