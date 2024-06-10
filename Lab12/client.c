#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int client_socket;
int client_id;
struct sockaddr_in server_addr;
int child_pid;

void handle_sigint(int sig) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%d:STOP:", client_id);
    sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (child_pid > 0) {
        kill(child_pid, SIGTERM);
        waitpid(child_pid, NULL, 0);
        printf("KILLING CHILD PROCESS\n");
    }

    close(client_socket);
    exit(0);
}

void* receive_messages() {
    char buffer[BUFFER_SIZE];
    int bytes_read;
    while (1) {
        bytes_read = recvfrom(client_socket, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Received: %s\n", buffer);

            if (strcmp(buffer, "PING") == 0) {
                snprintf(buffer, BUFFER_SIZE, "%d:PONG:", client_id);
                sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr *) &server_addr, sizeof(server_addr));
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_address> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *server_address = argv[1];
    int server_port = atoi(argv[2]);

    char buffer[BUFFER_SIZE];

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_address);
    server_addr.sin_port = htons(server_port);

    signal(SIGINT, handle_sigint);

    snprintf(buffer, sizeof(buffer), "-1:INIT:");
    printf("Sending: %s\n", buffer);
    sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr *) &server_addr, sizeof(server_addr));

    int bytes_read = recvfrom(client_socket, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);
    if (bytes_read <= 0) {
        perror("recvfrom");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0';

    if (strstr(buffer, "Error") != NULL) {
        printf("%s\n", buffer);
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    client_id = atoi(buffer);
    printf("Client ID: %d\n", client_id);

    child_pid = fork();
    if (child_pid == 0) {
        receive_messages();
        exit(0);
    }

    while (fgets(buffer, BUFFER_SIZE, stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        char message[BUFFER_SIZE];
        snprintf(message, BUFFER_SIZE, "%d:%s", client_id, buffer);
        printf("Sending: %s\n", message);
        sendto(client_socket, message, strlen(message), 0, (struct sockaddr *) &server_addr, sizeof(server_addr));
    }

    close(client_socket);
    return 0;
}

