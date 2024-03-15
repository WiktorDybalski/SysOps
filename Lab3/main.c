#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void byte_reading(FILE *file, FILE *reversed_file, long file_size) {
    for (long i = file_size - 1; i >= 0; i--) {
        fseek(file, i, SEEK_SET);
        int byte = fgetc(file);
        fputc(byte, reversed_file);
    }
}

void big_byte_reading(FILE *file, FILE *reversed_file, long file_size) {
    int buffer_size = 1024;
    char buffer[buffer_size];
    long to_read = file_size;

    while (to_read >= buffer_size) {
        to_read -= buffer_size;
        fseek(file, to_read, SEEK_SET);
        fread(buffer, sizeof(char), buffer_size, file);

        for (int i = 0; i < buffer_size / 2; i++) {
            char temp = buffer[i];
            buffer[i] = buffer[buffer_size - i - 1];
            buffer[buffer_size - i - 1] = temp;
        }
        fwrite(buffer, sizeof(char), buffer_size, reversed_file);
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

int main() {
    FILE *file, *reversed_file, *reversed_file4;
    long file_size;

    file = fopen("file2", "rb");
    if (file == NULL) {
        perror("Open file error!");
        return 1;
    }

    reversed_file = fopen("reversed_file3", "wb");
    reversed_file4 = fopen("reversed_file4", "wb");
    if (reversed_file == NULL) {
        perror("Nie można otworzyć pliku docelowego");
        fclose(file);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    big_byte_reading(file, reversed_file, file_size);
    fclose(file);
    fclose(reversed_file);
    reversed_file = fopen("reversed_file3", "rb");
    byte_reading(reversed_file, reversed_file4, file_size);

    return 0;
}