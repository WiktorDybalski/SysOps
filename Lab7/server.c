#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define SERVER_LIFESPAN 100
#define SERVER_QUEUE_NAME "/server-queue"
#define MAX_CLIENTS_NUMBER 4
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

typedef struct {
    char client_queue_name[64];
    int client_id;
} client_info;

client_info clients[MAX_CLIENTS_NUMBER];
int client_count = 0;

void handle_init(char *client_queue) {
    char msg[MSG_BUFFER_SIZE];
    mqd_t client_queue_sender = mq_open(client_queue, O_WRONLY);

    if (client_count >= MAX_CLIENTS_NUMBER) {
        sprintf(msg, "Error: Maximum number of clients (%d) reached.", MAX_CLIENTS_NUMBER);
        printf("Server error: Maximum number of clients reached: %d\n", MAX_CLIENTS_NUMBER);
        mq_send(client_queue_sender, msg, strlen(msg) + 1, 0);
        mq_close(client_queue_sender);
    } else {
        sprintf(clients[client_count].client_queue_name, "%s", client_queue);
        clients[client_count].client_id = client_count;
        sprintf(msg, "%d", client_count);
        printf("Creating client with client_id: %d\n", client_count);
        mq_send(client_queue_sender, msg, strlen(msg) + 1, 0);
        mq_close(client_queue_sender);
        client_count++;
    }
}

void broadcast_message(char *message, int sender_id) {
    printf("Broadcasting client message with Client_id: %d\n", sender_id);
    for (int i = 0; i < client_count; i++) {
        if (clients[i].client_id != sender_id) {
            mqd_t client_queue_broadcaster = mq_open(clients[i].client_queue_name, O_WRONLY);
            mq_send(client_queue_broadcaster, message, strlen(message) + 1, 0);
            mq_close(client_queue_broadcaster);
        }
    }
}

int main() {
    time_t server_start, current_server_time;
    mqd_t qd_server;
    char in_buffer[MSG_BUFFER_SIZE];
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mq_unlink(SERVER_QUEUE_NAME);
    qd_server = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr);

    printf("=================================== Server has started ==========================================\n");
    time(&server_start);
    while (time(&current_server_time) - server_start < SERVER_LIFESPAN) {
        mq_receive(qd_server, in_buffer, MSG_BUFFER_SIZE, NULL);
        printf("Received message: %s\n", in_buffer);
        char *token = strtok(in_buffer, ":");
        int client_id;
        if (strcmp(token, "INIT") == 0) {
            token = strtok(NULL, ":");
            handle_init(token);
        } else {
            client_id = atoi(token);
            token = strtok(NULL, ":");
            broadcast_message(token, client_id);
        }
    }

    mq_close(qd_server);
    mq_unlink(SERVER_QUEUE_NAME);
    printf("Server time: %d, has ended\n", SERVER_LIFESPAN);
    return 0;
}