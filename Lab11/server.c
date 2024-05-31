

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_CLIENTS_NUMBER 4
#define PING_INTERVAL 10
#define BUFFER_SIZE 1024

typedef struct {
    int client_id;
    int sockfd;
    time_t last_ping;
    int ping_waiting;
} client_info;

client_info clients[MAX_CLIENTS_NUMBER];
int client_count = 0;

void handle_init(int client_socket) {
    char msg[BUFFER_SIZE];
    if (client_count >= MAX_CLIENTS_NUMBER) {
        sprintf(msg, "Error: Maximum number of clients (%d) reached.", MAX_CLIENTS_NUMBER);
        printf("Server error: Maximum number of clients reached: %d\n", MAX_CLIENTS_NUMBER);
        send(client_socket, msg, strlen(msg), 0);
        close(client_socket);
    } else {
        clients[client_count].sockfd = client_socket;
        clients[client_count].client_id = client_count;
        sprintf(msg, "%d", client_count);
        printf("Creating client with client_id: %d\n", client_count);
        send(client_socket, msg, strlen(msg), 0);
        client_count++;
    }
}

void list(int client_socket) {
    char buffer[BUFFER_SIZE];
    int offset = snprintf(buffer, sizeof (buffer), "List of clients: \n");
    for (int i = 0; i < client_count; i++) {
        if (clients[i].sockfd != 0) {
            int bytes_written = snprintf(buffer + offset, sizeof(buffer) - offset, "Client %d\n", clients[i].client_id);
            offset += bytes_written;
            if (offset >= sizeof(buffer)) {
                break;
            }
        }
    }
    send(client_socket, buffer, offset, 0);
}

void to_all(int client_id, char* msg) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "Client with ID %d: %s", client_id, msg);
    for (int i = 0; i < client_count; i++) {
        if (clients[i].sockfd != 0 && clients[i].client_id != client_id) {
            send(clients[i].sockfd, buffer, strlen(buffer), 0);
        }
    }
}

void to_one(int sender_id, int receiver_id, char* msg) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "Client with ID %d: %s", sender_id, msg);
    for (int i = 0; i < client_count; i++) {
        if (clients[i].sockfd != 0 && clients[i].client_id == receiver_id) {
            send(clients[i].sockfd, buffer, strlen(buffer), 0);
            break;
        }
    }
}

void handle_stop(int client_id) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].sockfd != 0 && clients[i].client_id == client_id) {
            close(clients[i].sockfd);
            clients[i].sockfd = 0;
            client_count--;
            printf("Client with ID %d disconnected", client_id);
            break;
        }
    }
}

void alive() {
    time_t now = time(NULL);
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        if (clients[i].sockfd != 0) {
            if (clients[i].ping_waiting) {
                // Client did not respond to the last ping
                close(clients[i].sockfd);
                clients[i].sockfd = 0;
                client_count--;
                printf("Client %d removed due to no ping response\n", clients[i].client_id);
            } else {
                // Send ping
                char* ping_msg = "PING";
                send(clients[i].sockfd, ping_msg, strlen(ping_msg), 0);
                clients[i].ping_waiting = 1;
                clients[i].last_ping = now;
            }
        }
    }
}

void handle_client_message(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_read <= 0) {
        handle_stop(client_socket);
    } else {
        buffer[bytes_read] = '\0';
        char* token = strtok(buffer, " ");
        if (strcmp(token, "LIST") == 0) {
            list(client_socket);
        } else if (strcmp(token, "2ALL") == 0) {
            token = strtok(NULL, "");
            to_all(client_socket, token);
        } else if (strcmp(token, "2ONE") == 0) {
            int receiver_id = atoi(strtok(NULL, " "));
            token = strtok(NULL, "");
            to_one(client_socket, receiver_id, token);
        } else if (strcmp(token, "STOP") == 0) {
            handle_stop(client_socket);
        } else {
            printf("Unknown command received\n");
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <catcher PID> <mode>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char* address = argv[1];
    int port = atoi(argv[2]);

    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address);
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0) {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("=================================== Server has started ==========================================\n");
    printf("Address: %s, Port: %d\n", address, port);

    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            perror("accept");
            continue;
        }
    }
    close(server_socket);
    return 0;
}