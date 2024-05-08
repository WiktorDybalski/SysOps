#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <time.h>
#include <math.h>

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

struct timespec get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    return ts;
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
    struct timespec time_start = get_time();

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

    struct timespec end = get_time();
    long double execution_time = (end.tv_sec - time_start.tv_sec) + (end.tv_nsec - time_start.tv_nsec) / pow(10, 9);

    FILE *time_file = fopen("time_file.txt", "a");
    if (time_file == NULL) {
        printf("There is no such file");
    } else {
        fprintf(time_file, "Time: %Lf, Amount of processes: %d, Width: %.10lf\n", execution_time, number_of_proc, width);
        fclose(time_file);
    }

    printf("Width of sub-intervals: %.10lf, Number of processes: %d\n", width, number_of_proc);
    printf("Execution time: %.10Lf seconds\n", execution_time);
    printf("Integration result: %lf\n", total);
    return 0;
}
