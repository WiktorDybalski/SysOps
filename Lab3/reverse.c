#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 10

void byte_reading(FILE *file, FILE *reversed_file, long file_size) {
    for (long i = file_size - 1; i >= 0; i--) {
        fseek(file, i, SEEK_SET);
        int byte = fgetc(file);
        fputc(byte, reversed_file);
    }
}

void big_byte_reading(FILE *file, FILE *reversed_file, long file_size) {
    char buffer[BUFFER_SIZE];
    long to_read = file_size;

    while (to_read >= BUFFER_SIZE) {
        to_read -= BUFFER_SIZE;
        fseek(file, to_read, SEEK_SET);
        fread(buffer, sizeof(char), BUFFER_SIZE, file);

        for (int i = 0; i < BUFFER_SIZE / 2; i++) {
            char temp = buffer[i];
            buffer[i] = buffer[BUFFER_SIZE - i - 1];
            buffer[BUFFER_SIZE - i - 1] = temp;
        }
        fwrite(buffer, sizeof(char), BUFFER_SIZE, reversed_file);
    }

    if (to_read > 0) {
        fseek(file, 0, SEEK_SET);
        fread(buffer, sizeof(char), to_read, file);

        for (int i = 0; i < to_read / 2; i++) {
            char temp = buffer[i];
            buffer[i] = buffer[to_read - i - 1];
            buffer[to_read - i - 1] = temp;
        }
        fwrite(buffer, sizeof(char), to_read, reversed_file);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s input_file output_file\n", argv[0]);
        return 1;
    }

    FILE *time_file;
    long file_size;
    clock_t start, end;
    double cpu_time_used;
    FILE *file = fopen(argv[1], "rb");

    if (file == NULL) {
        perror("Opening file error!");
        return 1;
    }
    time_file = fopen("pomiar_zad_2.txt", "a");
    if (time_file == NULL) {
        perror("Opening time file error!");
        fclose(file);
        return 1;
    }
    FILE *reversed_file = fopen(argv[2], "wb");
    if (reversed_file == NULL) {
        perror("Opening file error");
        fclose(file);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    #if defined(BYTES)
    start = clock();
    byte_reading(file, reversed_file, file_size);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    fprintf(time_file, "Czas wykonania (1 bajt): %f sekund\n", cpu_time_used);

    #elif !defined(BYTES)
    start = clock();
    big_byte_reading(file, reversed_file, file_size);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    fprintf(time_file, "Czas wykonania (1024 bajty): %f sekund\n", cpu_time_used);
    #endif

    fclose(reversed_file);
    fclose(file);
    fclose(time_file);
    return 0;
}