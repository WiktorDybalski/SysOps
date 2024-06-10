#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define MAX_CLIENTS_NUMBER 4
#define PING_INTERVAL 10
#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

typedef struct {
    int client_id;
    struct sockaddr_in client_addr;
    socklen_t addr_len;
    int ping_waiting;
    time_t last_ping;
} client_info;

client_info clients[MAX_CLIENTS_NUMBER];
int client_count = 0;
int server_socket;

void init_client_info() {
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        clients[i].client_id = -1;
        clients[i].addr_len = sizeof(struct sockaddr_in);
        clients[i].ping_waiting = 0;
        clients[i].last_ping = 0;
    }
}

void handle_init(struct sockaddr_in *client_addr, socklen_t addr_len) {
    char msg[BUFFER_SIZE];
    if (client_count >= MAX_CLIENTS_NUMBER) {
        sprintf(msg, "Error: Maximum number of clients (%d) reached.", MAX_CLIENTS_NUMBER);
        sendto(server_socket, msg, strlen(msg), 0, (struct sockaddr *)client_addr, addr_len);
    } else {
        for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
            if (clients[i].client_id < 0) {
                clients[i].client_addr = *client_addr;
                clients[i].addr_len = addr_len;
                clients[i].client_id = i;
                clients[i].ping_waiting = 0;
                clients[i].last_ping = time(NULL);

                sprintf(msg, "%d", i);
                sendto(server_socket, msg, strlen(msg), 0, (struct sockaddr *)client_addr, addr_len);
                client_count++;
                break;
            }
        }
    }
}

void list_clients(int socket, struct sockaddr_in *client_addr, socklen_t addr_len) {
    char buffer[BUFFER_SIZE];
    int offset = snprintf(buffer, sizeof(buffer), "List of clients: \n");
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        if (clients[i].client_id >= 0) {
            int bytes_written = snprintf(buffer + offset, sizeof(buffer) - offset, "Client %d\n", clients[i].client_id);
            offset += bytes_written;
            if (offset >= sizeof(buffer)) {
                break;
            }
        }
    }
    sendto(socket, buffer, offset, 0, (struct sockaddr *)client_addr, addr_len);
}

void format_time(char *buffer, size_t buffer_size, time_t time_val) {
    struct tm *tm_info = localtime(&time_val);
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", tm_info);
}

void to_all(int client_id, char *msg) {
    char buffer[BUFFER_SIZE];
    char time_buffer[BUFFER_SIZE];
    format_time(time_buffer, sizeof(time_buffer), time(NULL));
    snprintf(buffer, sizeof(buffer), "Date: %s, Client with ID %d: %s", time_buffer, client_id, msg);
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        if (clients[i].client_id >= 0 && clients[i].client_id != client_id) {
            sendto(server_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&clients[i].client_addr, clients[i].addr_len);
        }
    }
}

void to_one(int sender_id, int receiver_id, char *msg) {
    char buffer[BUFFER_SIZE];
    char time_buffer[BUFFER_SIZE];
    format_time(time_buffer, sizeof(time_buffer), time(NULL));
    snprintf(buffer, sizeof(buffer), "Date: %s, Client with ID %d: %s", time_buffer, sender_id, msg);
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        if (clients[i].client_id == receiver_id) {
            sendto(server_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&clients[i].client_addr, clients[i].addr_len);
            break;
        }
    }
}

void handle_stop(struct sockaddr_in *client_addr, socklen_t addr_len) {
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        if (clients[i].client_id >= 0 && memcmp(&clients[i].client_addr, client_addr, addr_len) == 0) {
            clients[i].client_id = -1;
            client_count--;
            break;
        }
    }
}

void alive() {
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        if (clients[i].client_id >= 0) {
            if (clients[i].ping_waiting && difftime(time(NULL), clients[i].last_ping) >= PING_INTERVAL) {
                // Client did not respond to PING, remove it
                printf("Client %d did not respond to PING, removing...\n", clients[i].client_id);
                clients[i].client_id = -1;
                client_count--;
            } else if (!clients[i].ping_waiting) {
                // Send PING message
                char *ping_msg = "PING";
                sendto(server_socket, ping_msg, strlen(ping_msg), 0, (struct sockaddr *)&clients[i].client_addr, clients[i].addr_len);
                clients[i].ping_waiting = 1;
                clients[i].last_ping = time(NULL);
            }
        }
    }
}

void handle_client_message(char *buffer, struct sockaddr_in *client_addr, socklen_t addr_len) {
    printf("Received message: %s\n", buffer);
    int client_id = atoi(strtok(buffer, ":"));
    char *token = strtok(NULL, ":");
    if (strcmp(token, "PONG") == 0) {
        for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
            if (clients[i].client_id >= 0 && memcmp(&clients[i].client_addr, client_addr, addr_len) == 0) {
                clients[i].ping_waiting = 0;
                clients[i].last_ping = time(NULL);
                break;
            }
        }
    } else if (strcmp(token, "LIST") == 0) {
        list_clients(server_socket, client_addr, addr_len);
    } else if (strcmp(token, "INIT") == 0) {
        handle_init(client_addr, addr_len);
    } else if (strcmp(token, "2ALL") == 0) {
        token = strtok(NULL, ":");
        to_all(client_id, token);
    } else if (strcmp(token, "2ONE") == 0) {
        int receiver_id = atoi(strtok(NULL, ":"));
        token = strtok(NULL, ":");
        to_one(client_id, receiver_id, token);
    } else if (strcmp(token, "STOP") == 0) {
        handle_stop(client_addr, addr_len);
    } else {
        printf("Unknown command received\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_address> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *address = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address);
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    init_client_info();

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];

    event.events = EPOLLIN;
    event.data.fd = server_socket;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &event) == -1) {
        perror("epoll_ctl");
        close(server_socket);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    time_t current_time;
    time_t last_ping_time = time(NULL);

    char buffer[BUFFER_SIZE];
    printf("=================================== Server has started ==========================================\n");
    printf("Address: %s, Port: %d\n", address, port);

    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000); // 1 second timeout
        if (n == -1) {
            perror("epoll_wait");
            close(server_socket);
            close(epoll_fd);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == server_socket) {
                int bytes_read = recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &client_addr, &client_addr_len);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    handle_client_message(buffer, &client_addr, client_addr_len);
                }
            }
        }

        current_time = time(NULL);
        if (difftime(current_time, last_ping_time) >= PING_INTERVAL) {
            alive();
            last_ping_time = current_time;
        }
    }

    close(server_socket);
    close(epoll_fd);
    return 0;
}
