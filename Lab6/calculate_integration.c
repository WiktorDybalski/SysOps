#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#define FIFO_PATH "/tmp/my_fifo"

double func(double x) {
    return 4.0 / (x * x + 1);
}

double calculate_small_interval(double start, double end, double width) {
    double sum = 0.0;
    for (double x = start; x < end; x += width) {
        sum += func(x) * width;
    }
    return sum;
}

int main() {
    double start, end;
    int fifo_fd = open(FIFO_PATH, O_RDONLY);
    if (fifo_fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    read(fifo_fd, &start, sizeof(start));
    read(fifo_fd, &end, sizeof(end));
    close(fifo_fd);
    double result = calculate_small_interval(start, end, 0.000001);

    fifo_fd = open(FIFO_PATH, O_WRONLY);
    write(fifo_fd, &result, sizeof(result));
    close(fifo_fd);
}