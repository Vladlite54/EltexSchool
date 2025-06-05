#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <sys/epoll.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define MAX_DRIVERS 100
#define MAX_EVENTS 10
#define CLI_QUEUE_NAME "/cli_queue"

const char *command_list =
"Commands:\n"
"create_driver\n"
"send_task <pid> <task_timer>\n"
"get_status <pid>\n"
"get_drivers\n"
"show_commands\n"
"exit\n";

typedef struct Driver { // Структура, хранящая данные о водителе (его pid и его очередь)
    pid_t pid;
    mqd_t mq;
} Driver;

Driver drivers[MAX_DRIVERS];    // Массив водителей 
int drivers_count = 0;

mqd_t cli_queue;    // Дескриптор главной очереди
int epoll_base;     // Дескриптор epoll

int find_driver(pid_t pid); // Поиск водителя по его pid
void close_ds();    // Закрыть все дескрипторы

// Основные команды 
void create_driver();
void send_task(pid_t pid, int time_sec);
void get_status(pid_t pid);
void get_drivers();

void command_handler(const char *command);  // Обработчик команд

int main(int argc, char *argv[]) {
    struct mq_attr attr = {
        .mq_flags = O_NONBLOCK,
        .mq_maxmsg = 10,
        .mq_msgsize = BUFFER_SIZE
    };

    cli_queue = mq_open(CLI_QUEUE_NAME, O_CREAT | O_RDONLY | O_NONBLOCK, 0666, &attr);

    epoll_base = epoll_create1(0);
    if (epoll_base == -1) {
        perror("epoll create error!");
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = cli_queue;

    if (epoll_ctl(epoll_base, EPOLL_CTL_ADD, cli_queue, &ev) == -1) {
        perror("epoll ctl error for cli queue!");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;

    if (epoll_ctl(epoll_base, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
        perror("epoll ctl error for stdin!");
        exit(EXIT_FAILURE);
    }
    
    struct epoll_event events[MAX_EVENTS];
    char command[BUFFER_SIZE];
    printf("%s", command_list);

    while (1) {
        int nfds = epoll_wait(epoll_base, events, MAX_EVENTS, -1);  // Отслеживаем события на главной очереди и потоке ввода

        if (nfds == -1) {
            perror("epoll_wait error");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == STDIN_FILENO) {
                if (fgets(command, sizeof(command), stdin) != NULL) {
                    command[strlen(command) - 1] = '\0';
                    command_handler(command);
                }
            }
            else if (events[i].data.fd == cli_queue) {
                char driver_message[BUFFER_SIZE];
                ssize_t bytes = mq_receive(cli_queue, driver_message, sizeof(driver_message), NULL);
                if (bytes > 0) {
                    printf(">>> %s\n", driver_message);
                }
                else if (errno != EAGAIN) {
                    perror("mq_receive error (cli queue)");
                }
            }
        }
    }

    return 0;
}

int find_driver(pid_t pid) {
    for (int i = 0; i < drivers_count; i++) {
        if (drivers[i].pid == pid) return i;
    }
    return -1;
}

void close_ds() {
    for (int i = 0; i < drivers_count; i++) {
        if (mq_send(drivers[i].mq, "exit", 4, 0) == -1) {
            perror("mq_send error (cli)");
        }

        usleep(100000);

        mq_close(drivers[i].mq);
        char mq_name[32];
        snprintf(mq_name, sizeof(mq_name), "/queue_%d", drivers[i].pid);
        mq_unlink(mq_name);
    }
    mq_close(cli_queue);
    mq_unlink(CLI_QUEUE_NAME);
    close(epoll_base);
}

void create_driver() {
    if (drivers_count >= MAX_DRIVERS) {
        printf("To much drivers!\n");
        return;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork error!");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        execlp("./Driver", "Driver", NULL);
        perror("execlp error");
        exit(1);
    }
    else {
        char queue_name[32];
        snprintf(queue_name, sizeof(queue_name), "/queue_%d", pid);

        usleep(100000);

        mqd_t mq = mq_open(queue_name, O_WRONLY | O_NONBLOCK);
        if (mq == -1) {
            perror("mq open error in parent!");
            return;
        }

        drivers[drivers_count].pid = pid;
        drivers[drivers_count].mq = mq;
        drivers_count++;
    }
}

void send_task(pid_t pid, int time_sec) {
    int index = find_driver(pid);
    if (index == -1) {
        printf("No shuch driver!\n");
        return;
    }
    else if (time_sec <= 0) {
        printf("Incorrect time!\n");
        return;
    }

    char task_msg[32];
    snprintf(task_msg, sizeof(task_msg), "task %d", time_sec);

    if (mq_send(drivers[index].mq, task_msg, sizeof(task_msg), 0) == -1) {
        perror("mq_send error (cli)");
    }
}

void get_status(pid_t pid) {
    int index = find_driver(pid);
    if (index == -1) {
        printf("No shuch driver!\n");
        return;
    }

    if (mq_send(drivers[index].mq, "status", 6, 0) == -1) {
        perror("mq_send error (cli)");
    }
}

void get_drivers() {
    if (drivers_count == 0) {
        printf("There are no drivers\n");
        return;
    }

    for (int i = 0; i < drivers_count; i++) {
        if (mq_send(drivers[i].mq, "status", 6, 0) == -1) {
            perror("mq_send error (cli)");
        }
        usleep(10000);
    }
}

void command_handler(const char *command) {
    char cmd_name[20];
    pid_t pid;
    int time_sec;

    if (strcmp(command, "exit") == 0) {
        close_ds();
        exit(EXIT_SUCCESS);
    }
    if (strcmp(command, "show_commands") == 0) {
        printf("%s", command_list);
    }
    else if (strcmp(command, "create_driver") == 0) {
        create_driver();
    }
    else if (strncmp(command, "get_status", 10) == 0) {
        if (sscanf(command, "%*s %d", &pid) == 1) {
            get_status(pid);
        };
    }
    else if (strncmp(command, "send_task", 9) == 0) {
        if (sscanf(command, "%*s %d %d", &pid, &time_sec) == 2) {
            send_task(pid, time_sec);
        }
    }
    else if (strcmp(command, "get_drivers") == 0) {
        get_drivers();
    }
    else {
        printf("Unknown command!\n");
    }
}
