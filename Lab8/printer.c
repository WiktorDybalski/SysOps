
#include <stdio.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>
#include <stdbool.h>
#include "config.h"

volatile bool end = false;

void signal_handler(int signum) {
    end = true;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <number_of_users>\n", argv[0]);
        return -1;
    }
    int number_of_printers = atoi(argv[1]);
    if (number_of_printers > MAX_PRINTERS) {
        printf("Error: Maximum number of printers is %d\n", MAX_PRINTERS);
        return -1;
    }

    int shm_fd = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open");
        return -1;
    }
    if (ftruncate(shm_fd, sizeof(PrintQueue)) < 0) {
        perror("ftruncate");
        return -1;
    }
    PrintQueue *queue = mmap(0, sizeof(PrintQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (queue == MAP_FAILED) {
        perror("mmap");
        return -1;
    }
    close(shm_fd);

    queue->number_of_printers = number_of_printers;
    for (int i = 0; i < number_of_printers; i++) {
        sem_init(&queue->printers[i].printer_semaphore, 1, 1);
        queue->printers[i].printer_state = WAITING;
        queue->printers[i].printer_buffer_size = 0;
    }

    signal(SIGINT, signal_handler);

    printf("Starting printing process!\n");
    for (int i = 0; i < number_of_printers; i++) {
        pid_t printer_pid = fork();
        if (printer_pid == 0) {
            while (end == false) {

                if(queue->printers[i].printer_state == PRINTING) {
                    printf("Printer %d (PID: %d): Printing!\n", i, getpid());
                    fflush(stdout);
                    for (int j = 0; j < queue->printers[i].printer_buffer_size; j++) {
                        sleep(1);
                        printf("%c", queue->printers[i].printer_buffer[j]);
                        fflush(stdout);
                    }
                    printf("\n");
                    printf("Printing using printer with id: %d done\n", getpid());
                    fflush(stdout);
                    sem_post(&queue->printers[i].printer_semaphore);
                    queue->printers[i].printer_state = WAITING;
                    printf("Printer %d with id: %d is waiting for requests\n", i, getpid());
                    fflush(stdout);
                }
            }
            exit(0);
        }
    }
    while (wait(NULL) > 0);
    munmap(queue, sizeof(PrintQueue));
    shm_unlink(SHARED_MEMORY);
}