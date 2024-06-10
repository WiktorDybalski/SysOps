#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_PATH "/tmp/my_fifo"

int main() {
    double start, end;
    printf("Put the integration interval: ");
    scanf("%lf %lf", &start, &end);
    double result = 0.0;

    mkfifo(FIFO_PATH, 0666);
    int fifo_fd = open(FIFO_PATH, O_WRONLY);
    if (fifo_fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    write(fifo_fd, &start, sizeof(double));
    write(fifo_fd, &end, sizeof(double));

    close(fifo_fd);

    fifo_fd = open(FIFO_PATH, O_RDONLY);
    read(fifo_fd, &result, sizeof(double));
    close(fifo_fd);

    printf("Integration result: %lf\n", result);
}