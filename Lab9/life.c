#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

int num_threads;

typedef struct {
    int start;
    int end;
    char **current;
    char **next;
} ThreadData;

void signal_handler(int signum) {}

void *compute_next_state(void *arg) {
    ThreadData *data = (ThreadData *) arg;
    while (true) {
        pause();

        for (int i = data->start; i < data->end; i++) {
            int row = i / GRID_WIDTH;
            int col = i % GRID_WIDTH;
            (*data->next)[i] = is_alive(row, col, *data->current);
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    num_threads = atoi(argv[1]);

    struct sigaction action;
    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGUSR1, &action, NULL);

    srand(time(NULL));
    setlocale(LC_CTYPE, "");
    initscr(); // Start curses mode

    char *foreground = create_grid();
    char *background = create_grid();
    char *tmp;

    pthread_t threads[num_threads];
    ThreadData data[num_threads];

    int cell_per_thread = (GRID_HEIGHT * GRID_WIDTH / num_threads);

    for (int i = 0; i < num_threads; i++) {
        data[i].start = i * cell_per_thread;
        if ((i + 1) * cell_per_thread > GRID_HEIGHT * GRID_WIDTH) {
            data[i].end = GRID_HEIGHT * GRID_WIDTH;
        } else {
            data[i].end = (i + 1) * cell_per_thread;
        }

        data[i].current = &foreground;
        data[i].next = &background;

        pthread_create(&threads[i], NULL, compute_next_state, &data[i]);
    }
    init_grid(foreground);

    while (true) {
        draw_grid(foreground);
        usleep(500 * 1000);

        for(int i = 0; i < num_threads; i++) {
            pthread_kill(threads[i], SIGUSR1);
        }

        // Step simulation
        update_grid(foreground, background);
        tmp = foreground;
        foreground = background;
        background = tmp;
    }

    endwin(); // End curses mode
    destroy_grid(foreground);
    destroy_grid(background);
    return 0;
}
