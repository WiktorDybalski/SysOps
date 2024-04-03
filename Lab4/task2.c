#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int global = 0;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Wrong number of arguments: Failed\n");
        return -1;
    }
    int local = 0;
    printf("Program name: %s\n", argv[0]);
    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
        printf("\nChild process\n");
        global++;
        local++;
        printf("Child pid = %d, parrent pid: %d\n", getpid(), getppid());
        printf("child's local = %d, child's global = %d\n", local, global);

        int exec_status = execl("/bin/ls", "ls", argv[1], NULL);
        if (exec_status == -1) {
            perror("execl failed");
            exit(EXIT_FAILURE);
        }
    } else {
        int status;
        waitpid(child_pid, &status, 0);
        printf("\nParent process\n");
        printf("Parent pid = %d, child pid = %d\n", getpid(), child_pid);
        printf("Child exit code: %d\n", WEXITSTATUS(status));
        printf("Parent's local = %d, parent's global = %d\n", local, global);
    }
    return EXIT_SUCCESS;
}
