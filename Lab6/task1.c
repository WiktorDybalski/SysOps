
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <sys/time.h>

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

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <width> <number of processes>\n", argv[0]);
        return 1;
    }

    double width = atof(argv[1]);
    int number_of_proc = atoi(argv[2]);
    double total = 0.0;
    int pipes[number_of_proc][2];

    struct timeval time_start, time_end;
    gettimeofday(&time_start, NULL);

    for (int i = 0; i < number_of_proc; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        pid_t pid = fork();
        if (pid == 0) {
            close(pipes[i][0]);

            double start = i * (1.0 / number_of_proc);
            double end = (i + 1) * (1.0 / number_of_proc);
            double result = calculate_small_interval(start, end, width);
            write(pipes[i][1], &result, sizeof(result));
            close(pipes[i][1]);
            exit(0);
        } else {
            close(pipes[i][1]);
        }
    }

    for (int i = 0; i < number_of_proc; i++) {
        double result;
        read(pipes[i][0], &result, sizeof(result));
        total += result;
        close(pipes[i][0]);
    }
    gettimeofday(&time_end, NULL);
    double execution_time = (time_end.tv_sec - time_start.tv_sec) + (time_end.tv_usec - time_start.tv_usec) / 1000000.0;
    for (int i = 0; i < number_of_proc; i++) {
        wait(NULL);
    }

    printf("Width of sub-intervals: %lf, Number of processes: %d\n", width, number_of_proc);
    printf("Execution time: %.6f seconds\n", execution_time);
    printf("Integration result: %lf\n", total);
}