#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>

int main() {

    DIR *curr_dir = opendir("dir_to_check");
    if (curr_dir == NULL) {
        perror("Nie mozna otworzyc folderu");
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(curr_dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    closedir(curr_dir);
    return 0;
}