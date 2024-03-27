#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int waiting_for_ack = 0;

void handle_sigusr1(int sig) {
    waiting_for_ack = 0; // Otrzymano potwierdzenie
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Użycie: %s <PID catchera> <tryb pracy>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int catcher_pid = atoi(argv[1]);
    int mode = atoi(argv[2]);
    union sigval value;
    value.sival_int = mode; // Przesyłanie trybu pracy

    // Konfiguracja obsługi sygnału SIGUSR1
    signal(SIGUSR1, handle_sigusr1);

    // Wysyłanie sygnału SIGUSR1 do catchera
    waiting_for_ack = 1;
    if (sigqueue(catcher_pid, SIGUSR1, value) < 0) {
        perror("Błąd wysyłania sygnału");
        exit(EXIT_FAILURE);
    }

    // Czekanie na potwierdzenie
    while (waiting_for_ack) {
        pause();
    }

    return 0;
}
