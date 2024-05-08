#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <wait.h>
#include <time.h>
#include <stdbool.h>
#include "config.h"


volatile bool end = false;

void signal_handler(int signum) {
    end = true;
}

void generate_random_string(char* buffer, int n) {
    for (int i = 0; i < n; i++) {
        buffer[i] = 'a' + rand() % 26;
    }
    buffer[n] = '\0';
}

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Usage: %s <number_of_users>\n", argv[0]);
        return -1;
    }
    int number_of_users = atoi(argv[1]);

    int shm_fd = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open");
        return -1;
    }

    PrintQueue *queue = mmap(0, sizeof(PrintQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (queue == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    signal(SIGINT, signal_handler);

    if (queue->number_of_printers <= 0) {
        printf("Error: No printers available!\n");
        munmap(queue, sizeof(PrintQueue));
        return -1;
    }

    printf("Creating clients!\n");
    for (int i = 0; i < number_of_users; i++) {
        pid_t client_pid = fork();
        if (client_pid == 0) {
            char buffer[PRINT_SIZE + 1];
            srand(time(NULL) ^ (getpid() << 16));
            while (!end){
                sleep(rand() % 5 + 3);
                generate_random_string(buffer, PRINT_SIZE);

                int printer_index = -1;
                for (int j = 0; j < queue->number_of_printers; j++) {
                    int val;
                    sem_getvalue(&queue->printers[j].printer_semaphore, &val);
                    if(val > 0) {
                        printer_index = j;
                        break;
                    }
                }
                if (printer_index == -1) {
                    printer_index = rand() % queue->number_of_printers;
                }
                if (sem_wait(&queue->printers[printer_index].printer_semaphore)) {
                    perror("sem_wait");
                }
                strcpy(queue->printers[printer_index].printer_buffer, buffer);
                queue->printers[printer_index].printer_buffer_size = PRINT_SIZE;
                printf("Sending printing request to printer number: %d\n", printer_index);
                fflush(stdout);
                queue->printers[printer_index].printer_state = PRINTING;
            }
            exit(0);
        }
    }
    while (wait(NULL) > 0);
    munmap(queue, sizeof(PrintQueue));
    shm_unlink(SHARED_MEMORY);
    return 0;
}