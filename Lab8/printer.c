
#include <stdio.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define QUEUE_LEN  10
#define PRINT_SIZE  11
#define SHARED_MEMORY "shared_memory"

typedef struct {
    char jobs[QUEUE_LEN][PRINT_SIZE];
    int put_index;
    int take_index;
} PrintQueue;

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <number_of_users>\n", argv[0]);
        return -1;
    }
    int number_of_printers = atoi(argv[1]);
    int shm_fd = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(PrintQueue));
    close(shm_fd);
    PrintQueue *queue = mmap(0, sizeof(PrintQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    char *job;
    while (1) {
        strcpy(job, queue->jobs[queue->take_index]);
        queue->take_index = (queue->take_index + 1) % QUEUE_LEN;
        printf("Printer id: %d: Printing!", getppid());
        for (int i = 0; i < PRINT_SIZE - 1; i++) {
            printf("%c\n", queue->jobs[queue->take_index][i]);
        }
    }
}