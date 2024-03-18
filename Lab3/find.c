#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

int main() {
    DIR *curr_dir = opendir(".");
    if (curr_dir == NULL) {
        perror("Opening Folder Error");
        return -1;
    }

    long long total_size = 0;
    struct dirent *entry;
    struct stat fileStat;

    while ((entry = readdir(curr_dir)) != NULL) {
        if (stat(entry->d_name, &fileStat) ==  -1) {
                perror("stat error");
                continue;
        }
        if(!S_ISDIR(fileStat.st_mode)) {
            printf("File: %s, Size: %lld bytes\n", entry->d_name, (long long)fileStat.st_size);
            total_size += fileStat.st_size;
        }
    }
    closedir(curr_dir);
    printf("\nTotal size: %lld\n", total_size);
    return 0;
}
