#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>


#define SERVER_QUEUE_NAME "/server-queue"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

char client_queue_name[64];
struct sigevent sigev;

void handle_message(int signum) {
    mqd_t qd_client;
    char in_buffer[MSG_BUFFER_SIZE];
    qd_client = mq_open(client_queue_name, O_RDONLY);
    mq_receive(qd_client, in_buffer, MSG_BUFFER_SIZE, NULL);
    printf("Received: %s\n", in_buffer);

    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    sigev.sigev_value.sival_ptr = &qd_client;
    if (mq_notify(qd_client, &sigev) == -1) {
        perror("mq_notify failed");
    }
}

int main() {
    mqd_t qd_client, qd_server;
    char out_buffer[MSG_BUFFER_SIZE];
    char in_buffer[MSG_BUFFER_SIZE];
    struct mq_attr attr;


    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    sprintf(client_queue_name, "/client-queue-%d", getpid());
    mq_unlink(client_queue_name);
    qd_client = mq_open(client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr);

    qd_server = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    sprintf(out_buffer, "INIT:%s", client_queue_name);
    mq_send(qd_server, out_buffer, strlen(out_buffer) + 1, 0);

    mq_receive(qd_client, in_buffer, MSG_BUFFER_SIZE, NULL);
    if (strstr(in_buffer, "Error") != NULL) {
        printf("%s\n", in_buffer);
        mq_close(qd_client);
        mq_unlink(client_queue_name);
        mq_close(qd_server);
        exit(EXIT_FAILURE);
    }
    int client_id = atoi(in_buffer);
    printf("Client ID: %d\n", client_id);

    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    sigev.sigev_value.sival_ptr = &qd_client;
    signal(SIGUSR1, handle_message);
    mq_notify(qd_client, &sigev);

    while (fgets(out_buffer, MSG_BUFFER_SIZE, stdin)) {
        size_t len = strlen(out_buffer);
        if (len > 0 && out_buffer[len - 1] == '\n') {
            out_buffer[len - 1] = '\0';
        }
        sprintf(in_buffer, "%d:%s", client_id, out_buffer);
        mq_send(qd_server, in_buffer, strlen(in_buffer) + 1, 0);
        mq_notify(qd_client, &sigev);
    }
    mq_close(qd_client);
    mq_unlink(client_queue_name);
    mq_close(qd_server);

    return 0;
}