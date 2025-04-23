#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>

#define QUEUE_NAME "/posix_chat_queue"
#define MAX_MSG_SIZE 256
#define MSG_PRIO_NORMAL 1
#define MSG_PRIO_EXIT 255

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];
    int exit_flag = 0;
    
    // ��������� ��������� �������
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // ��������� ������� (������ ���� ��� ������� ������ ����������)
    mq = mq_open(QUEUE_NAME, O_RDWR);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    printf("Session started\n");

    while (!exit_flag) {
        // ���������� ������ ���������
        printf("Enter message ->: ");
        fgets(buffer, MAX_MSG_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        // ���������, �� ����� �� ������������ �����
        if (strcmp(buffer, "exit") == 0) {
            if (mq_send(mq, buffer, strlen(buffer)+1, MSG_PRIO_EXIT) == -1) {
                perror("mq_send exit");
            }
            exit_flag = 1;
        } else {
            if (mq_send(mq, buffer, strlen(buffer)+1, MSG_PRIO_NORMAL) == -1) {
                perror("mq_send");
                break;
            }
        }

        if (exit_flag) break;

        // �������� �����
        printf("Waiting...\n");
        unsigned int prio;
        if (mq_receive(mq, buffer, MAX_MSG_SIZE, &prio) == -1) {
            perror("mq_receive");
            break;
        }

        printf("From A: %s\n", buffer);

        // �������� �� ��������� � ������
        if (strcmp(buffer, "exit") == 0 || prio == MSG_PRIO_EXIT) {
            exit_flag = 1;
        }
    }

    // ��������� �������
    mq_close(mq);

    return 0;
}