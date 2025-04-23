#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define MAX_SIZE 128
#define TERMINATE_TYPE 255

typedef struct message {
    long type;
    char text[MAX_SIZE];
} message;

int main(int argc, char *argv[]) {
    key_t key = ftok(".", 'A');
    int msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1) {
        perror("Queue get error\n");
        exit(1);
    }
    printf("Session started\n");

    message msg;
    int running = 1;

    strcpy(msg.text, "B has been connected\n");
    msg.type = 2;

    if (msgsnd(msqid, &msg, sizeof(msg), 0) < 0) {
        perror("Send error\n");
        exit(1);
    }

    while (running) {
        if (msgrcv(msqid, &msg, sizeof(msg), 1, 0) < 0) {
            perror("Receive error\n");
            exit(1);
        }

        printf("From A: %s\n", msg.text);

        if (strncmp(msg.text, "exit", 4) == 0) {
            printf("Termination signal received. Exiting...\n");
            running = 0;
            break;
        }

        printf("Enter message -> ");
        fgets(msg.text, MAX_SIZE, stdin);
        msg.type = 2;
        
        if (msgsnd(msqid, &msg, sizeof(msg), 0) < 0) {
            perror("Send error\n");
            exit(1);
        }

        if (strncmp(msg.text, "exit", 4) == 0) exit(0);
    }
    //msgctl(msqid, IPC_RMID, NULL);
    return 0;
}