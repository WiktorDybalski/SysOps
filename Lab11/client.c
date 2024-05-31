#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUFFER_SIZE 1024

int client_socket;
int client_id;
char client_name[BUFFER_SIZE];

void handle_sigint(int sig) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "STOP %d", client_id);
    send(client_socket, buffer, strlen(buffer), 0);
    close(client_socket);
    exit(0);
}

void receive_messages() {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        if (strncmp(buffer, "PING", 4) == 0) {
            send(client_socket, "PONG", 4, 0);
        } else {
            printf("%s\n", buffer);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <name> <server_address> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    strcpy(client_name, argv[1]);
    char* server_address = argv[2];
    int server_port = atoi(argv[3]);

    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_address);
    server_addr.sin_port = htons(server_port);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    snprintf(buffer, sizeof(buffer), "INIT %s", client_name);
    send(client_socket, buffer, strlen(buffer), 0);

    int bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read <= 0) {
        perror("recv");
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

    signal(SIGINT, handle_sigint);

    if (fork() == 0) {
        receive_messages();
        exit(0);
    }

    while (fgets(buffer, BUFFER_SIZE, stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        snprintf(buffer, BUFFER_SIZE, "%d %s", client_id, buffer);
        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket);
    return 0;
}
