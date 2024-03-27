#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int mode_change_requests = 0;

void handle_sigusr1(int sig, siginfo_t *info, void *ucontext) {
    mode_change_requests++;
    int sender_pid = info->si_pid;
    union sigval value;
    printf("Odebrano sygnał SIGUSR1 od PID: %d\n", sender_pid);

    switch (info->si_value.sival_int) {
        case 1:
            for (int i = 1; i <= 100; i++) {
                printf("%d\n", i);
            }
            break;
        case 2:
            printf("Liczba żądań zmiany trybu pracy: %d\n", mode_change_requests);
            break;
        case 3:
            printf("Zakończenie pracy.\n");
            exit(0);
        default:
            printf("Nieznany tryb pracy.\n");
    }

    // Wysyłanie potwierdzenia do sendera
    sigqueue(sender_pid, SIGUSR1, value);
}

int main() {
    printf("PID catchera: %d\n", getpid());

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    // Czekanie na sygnały
    while (1) {
        pause();
    }

    return 0;
}
