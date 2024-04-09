#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <time.h>

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
    int total_intervals = (int)(1.0 / width);
    int intervals_per_proc = total_intervals / number_of_proc;
    int remaining_intervals = total_intervals % number_of_proc;

    double total = 0.0;
    int pipes[number_of_proc][2];

    clock_t time_start, time_end;
    time_start = clock();

    for (int i = 0; i < number_of_proc; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        pid_t pid = fork();
        if (pid == 0) {
            close(pipes[i][0]);
            double start = i * intervals_per_proc * width;
            double end = (i + 1) * intervals_per_proc * width;
            if (i == number_of_proc - 1) {
                end += remaining_intervals * width;
            }
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

    for (int i = 0; i < number_of_proc; i++) {
        wait(NULL);
    }

    time_end = clock();
    double execution_time = ((double)(time_end - time_start)) / CLOCKS_PER_SEC;

    FILE *time_file = fopen("time_file.txt", "a");
    if (time_file == NULL) {
        printf("There is no such file");
    } else {
        fprintf(time_file, "Time: %lf, Amount of processes: %d, Width: %lf\n", execution_time, number_of_proc, width);
        fclose(time_file);
    }

    printf("Width of sub-intervals: %lf, Number of processes: %d\n", width, number_of_proc);
    printf("Execution time: %.6f seconds\n", execution_time);
    printf("Integration result: %lf\n", total);
    return 0;
}
