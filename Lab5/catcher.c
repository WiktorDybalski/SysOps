#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int mode = -1;
int number_of_requests = 0;

void SIGUSR1_action(int sig, siginfo_t *info, void *context) {
        pid_t sender_pid = info -> si_pid;
        int int_val = info->si_int;
        mode = info->si_value.sival_int;
        printf("Received request %d from sender with PID: %d\n", int_val, sender_pid);

        number_of_requests++;
        mode = int_val;

        kill(sender_pid, SIGUSR1);
}

int main() {
    printf("Catcher PID: %d\n", getpid());

    struct sigaction action;
    action.sa_sigaction = SIGUSR1_action;
    action.sa_flags = SA_SIGINFO;

    sigemptyset(&action.sa_mask);
    sigaction(SIGUSR1, &action, NULL);

    while (1) {
        switch(mode) {
            case 1:
                for (int i = 1; i <= 100; i++) {
                    printf("%i, ", i);
                }
                printf("\n");
                mode = -1;
                break;
            case 2:
                printf("Number of requests: %d\n", number_of_requests);
                mode = -1;
                break;
            case 3:
                printf("Exit signal\n");
                exit(EXIT_SUCCESS);
            default:
                printf("Catcher is waiting\n");
                pause();
                break;
        }
    }
}