
#include <stdio.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>
#include "config.h"


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <number_of_users>\n", argv[0]);
        return -1;
    }
    int number_of_printers = atoi(argv[1]);

    sem_t *empty = sem_open("/empty", O_CREAT, 0666, PRINT_SIZE);
    sem_t *full = sem_open("/full", O_CREAT, 0666, 0);
    sem_t *operation = sem_open("/usage", O_CREAT, 0666, 1);


    int shm_fd = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(PrintQueue));
    PrintQueue *queue = mmap(0, sizeof(PrintQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    close(shm_fd);

    printf("Starting printing process!\n");
    for (int i = 0; i < number_of_printers; i++) {
        pid_t printer_pid = fork();
        if (printer_pid == 0) {
            char job[PRINT_SIZE];
            while (1) {
                sem_wait(full);
                sem_wait(operation);

                printf("Handling print request using printer with id: %d \n", getppid());
                strcpy(job, queue->jobs[queue->take_index]);
                queue->take_index = (queue->take_index + 1) % QUEUE_LEN;

                printf("Printer with id: %d: Printing!\n", getpid());
                for (int i = 0; i < PRINT_SIZE; i++) {
                    sleep(1);
                    printf("%c", queue->jobs[queue->take_index][i]);
                    fflush(stdout);
                }
                printf("\n");

                sem_post(empty);
                sem_post(operation);
            }
        }
    }
    while (wait(NULL) > 0);
    sem_close(empty);
    sem_close(full);
    sem_close(operation);
    sem_unlink("/empty");
    sem_unlink("/full");
    sem_unlink("/usage");
    munmap(queue, sizeof(PrintQueue));
    shm_unlink(SHARED_MEMORY);
}