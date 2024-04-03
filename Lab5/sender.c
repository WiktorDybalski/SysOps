#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int sig) {
    printf("Signal Received from catcher: %d\n", sig);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <catcher PID> <mode>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    printf("Sender PID: %d\n", getpid());

    long catcher_pid = strtol(argv[1], NULL, 10);
    long mode = strtol(argv[2], NULL, 10);

    signal(SIGUSR1, signal_handler);
    union sigval value = {mode};
    sigqueue(catcher_pid, SIGUSR1, value);
    printf("Signal sent with value mode: %ld\n", mode);

    sigset_t mask;
    sigfillset(&mask);

    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGINT);

    sigsuspend(&mask);
}