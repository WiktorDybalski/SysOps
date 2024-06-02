#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#define MAX_CLIENTS_NUMBER 4
#define PING_INTERVAL 10
#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

typedef struct {
    int client_id;
    int sock_fd;
    int ping_waiting;
    time_t last_ping;
} client_info;

client_info clients[MAX_CLIENTS_NUMBER];
int client_count = 0;
int epoll_fd;

void init_client_info() {
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        clients[i].client_id = -1;
        clients[i].sock_fd = -1;
        clients[i].ping_waiting = 0;
        clients[i].last_ping = 0;
    }
}

void handle_init(int client_socket) {
    char msg[BUFFER_SIZE];
    if (client_count >= MAX_CLIENTS_NUMBER) {
        sprintf(msg, "Error: Maximum number of clients (%d) reached.", MAX_CLIENTS_NUMBER);
        printf("Server error: Maximum number of clients reached: %d\n", MAX_CLIENTS_NUMBER);
        send(client_socket, msg, strlen(msg), 0);
        close(client_socket);
    } else {
        for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
            if (clients[i].client_id < 0) {
                clients[i].sock_fd = client_socket;
                clients[i].client_id = i;

                struct epoll_event event;
                event.events = EPOLLIN;
                event.data.fd = client_socket;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event);

                sprintf(msg, "%d", i);
                printf("Creating client with client_id: %d, and sock_fd: %d\n", clients[i].client_id,
                       clients[i].sock_fd);
                send(client_socket, msg, strlen(msg), 0);
                client_count++;
                break;
            }
        }
    }
}

void list(int client_socket) {
    printf("Processing: LIST\n");
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
    printf("Sending list of clients:\n%s", buffer);
    send(client_socket, buffer, offset, 0);
}

void format_time(char *buffer, size_t buffer_size, time_t time_val) {
    struct tm *tm_info = localtime(&time_val);
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", tm_info);
}

void to_all(int client_id, char* msg) {
    printf("Processing: 2ALL\n");
    char buffer[BUFFER_SIZE];
    char time_buffer[BUFFER_SIZE];
    format_time(time_buffer, sizeof(time_buffer), time(NULL));
    snprintf(buffer, sizeof(buffer), "Date: %s, Client with ID %d: %s", time_buffer, client_id, msg);
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        if (clients[i].client_id >= 0 && clients[i].client_id != client_id) {
            send(clients[i].sock_fd, buffer, strlen(buffer), 0);
        }
    }
}

void to_one(int sender_id, int receiver_id, char *msg) {
    printf("Processing: 2ONE\n");
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "Client with ID %d: %s", sender_id, msg);
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        if (clients[i].client_id == receiver_id) {
            send(clients[i].sock_fd, buffer, strlen(buffer), 0);
            break;
        }
    }
}

void handle_stop(int client_socket) {
    printf("Processing: STOP\n");
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        if (clients[i].sock_fd == client_socket) {
            printf("Client with ID %d disconnected\n", clients[i].client_id);
            close(clients[i].sock_fd);
            clients[i].sock_fd = -1;
            clients[i].client_id = -1;
            client_count--;
            break;
        }
    }
}

void alive() {
    for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
        if (clients[i].sock_fd != -1) {
            if (clients[i].ping_waiting) {
                // Client did not respond to the last ping
                printf("Client %d removed due to no ping response\n", clients[i].client_id);
                close(clients[i].sock_fd);
                clients[i].sock_fd = -1;
                clients[i].client_id = -1;
                client_count--;
            } else {
                char *ping_msg = "PING";
                send(clients[i].sock_fd, ping_msg, strlen(ping_msg), 0);
                clients[i].ping_waiting = 1;
                clients[i].last_ping = time(NULL);
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
        int client_id = atoi(strtok(buffer, ":"));
        char *token = strtok(NULL, ":");
        printf("Received from ClientID %d, message: %s\n", client_id, token);
        if (strcmp(token, "PONG") == 0) {
            for (int i = 0; i < MAX_CLIENTS_NUMBER; i++) {
                if (clients[i].sock_fd == client_socket) {
                    clients[i].ping_waiting = 0;
                    clients[i].last_ping = time(NULL);
                    break;
                }
            }
        } else if (strcmp(token, "LIST") == 0) {
            list(client_socket);
        } else if (strcmp(token, "2ALL") == 0) {
            token = strtok(NULL, ":");
            to_all(client_id, token);
        } else if (strcmp(token, "2ONE") == 0) {
            int receiver_id = atoi(strtok(NULL, ":"));
            token = strtok(NULL, ":");
            to_one(client_id, receiver_id, token);
        } else if (strcmp(token, "STOP") == 0) {
            handle_stop(client_socket);
        } else {
            printf("Unknown command received\n");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_address> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *address = argv[1];
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

    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
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

    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLOUT;
    event.data.fd = server_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &event) < 0) {
        perror("epoll_ctl");
        close(server_socket);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    struct epoll_event events[MAX_EVENTS];
    init_client_info();
    time_t last_ping_time = time(NULL);
    while (1) {
        int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, PING_INTERVAL * 1000);
        if (event_count < 0) {
            perror("epoll_wait");
            close(server_socket);
            close(epoll_fd);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < event_count; i++) {
            if (events[i].data.fd == server_socket) {
                if (events[i].events & EPOLLIN) {
                    int client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_len);
                    if (client_socket < 0) {
                        perror("accept");
                        continue;
                    }
                    handle_init(client_socket);
                }
            } else {
                if (events[i].events & EPOLLIN) {
                    handle_client_message(events[i].data.fd);
                }
            }
        }
        time_t current_time = time(NULL);
        if (difftime(current_time, last_ping_time) >= PING_INTERVAL) {
            alive();
            last_ping_time = current_time;
        }
    }

    close(server_socket);
    close(epoll_fd);
    return 0;
}
