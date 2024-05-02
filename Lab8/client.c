#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <wait.h>
#include "config.h"


void generate_random_string(char* buffer, int n) {
    for (int i = 0; i < n; i++) {
        buffer[i] = 'a' + rand() % 26;
    }
    buffer[n] = '\n';
}

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Usage: %s <number_of_users>\n", argv[0]);
        return -1;
    }
    int number_of_users = atoi(argv[1]);

    int shm_fd = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666);
    PrintQueue *queue = mmap(0, sizeof(PrintQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t *empty = sem_open("/empty", O_CREAT, 0666, QUEUE_LEN);
    sem_t *full = sem_open("/full", O_CREAT, 0666, 0);
    sem_t *operation = sem_open("/usage", O_CREAT, 0666, 1);

    printf("Creating clients!\n");
    for (int i = 0; i < number_of_users; i++) {
        pid_t client_pid = fork();
        if (client_pid == 0) {
            char buffer[PRINT_SIZE + 1];
            while (1){
                generate_random_string(buffer, PRINT_SIZE);
                sem_wait(empty);
                sem_wait(operation);

                strcpy(queue->jobs[queue->put_index], buffer);
                queue->put_index = (queue->put_index + 1) % QUEUE_LEN;
                printf("Sending print request from client with id: %d\n", getppid());
                sem_post(full);
                sem_post(operation);
                sleep(1 + rand() % 3);
            }
        }
    }
    while (wait(NULL) > 0);  // Wait for all child processes
    return 0;
}