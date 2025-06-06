#define _POSIX_C_SOURCE 199309L 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <time.h>
#include <signal.h>
#include <stdatomic.h>

#define BUFFER_SIZE 1024
#define MAX_EVENTS 10
#define CLI_QUEUE_NAME "/cli_queue"

typedef enum status {  // Состояния водителя
    AVAILABLE,
    BUSY
} status;

status current_status;  // Текущий статус водителя

mqd_t my_queue; // Дескриптор очереди водителя
mqd_t cli_queue;  // Дескриптор главной очереди
int epoll_base; // Дескриптор epoll

timer_t timer_id;   // Таймер

void timer_callback(union sigval sv);   // Событие истечения таймера
void start_timer(unsigned int seconds); // Запуск таймера задачи

void send_init_msg();   // Сообщить главному процессу об активности

void send_status(); // Отправить главному процессу информацию о текущем статусе
void do_task(int time_sec); // Начать выполнять задачу

void request_handler(const char* request);  // Обработчик запросов главного процесса

int main(int agrc, char *argv[]) {
    current_status = AVAILABLE;
    pid_t my_pid = getpid();
    char my_queue_name[32];
    snprintf(my_queue_name, sizeof(my_queue_name), "/queue_%d", my_pid);

    struct mq_attr attr = {
        .mq_flags = O_NONBLOCK,
        .mq_maxmsg = 10,
        .mq_msgsize = BUFFER_SIZE
    };

    my_queue = mq_open(my_queue_name, O_CREAT | O_RDONLY | O_NONBLOCK, 0666, &attr);
    if (my_queue == (mqd_t)-1) {
        perror("mq_open (child)");
        exit(EXIT_FAILURE);
    }

    cli_queue = mq_open(CLI_QUEUE_NAME, O_WRONLY | O_NONBLOCK);
    if (cli_queue == (mqd_t)-1) {
        perror("mq_open (cli)");
        mq_close(my_queue);
        mq_unlink(my_queue_name);
        exit(EXIT_FAILURE);
    }

    epoll_base = epoll_create1(0);
    if (epoll_base == -1) {
        perror("epoll create error!");
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = my_queue;

    if (epoll_ctl(epoll_base, EPOLL_CTL_ADD, my_queue, &ev) == -1) {
        perror("epoll ctl error for cli queue!");
        exit(EXIT_FAILURE);
    }

    struct epoll_event events[MAX_EVENTS];

    send_init_msg();

    while(1) {
        int nfds = epoll_wait(epoll_base, events, MAX_EVENTS, -1);  // Обрабатываем события на очереди водителя

        if (nfds == -1) {
            perror("epoll_wait error");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == my_queue) {
                char cli_message[BUFFER_SIZE];
                memset(cli_message, 0, sizeof(cli_message));
                ssize_t bytes = mq_receive(my_queue, cli_message, sizeof(cli_message), NULL);
                if (bytes > 0) {
                    cli_message[strlen(cli_message)] = '\0';
                    request_handler(cli_message);
                }
                else if (errno != EAGAIN) {
                    perror("mq_receive error (driver)");
                }
            }
        }
        
    }

    return 0;
}

void timer_callback(union sigval sv) {
    current_status = AVAILABLE;
    char done_message[64];
    snprintf(done_message, sizeof(done_message), "Driver %d finished task", getpid());
    if (mq_send(cli_queue, done_message, sizeof(done_message), 0) == -1) {
        perror("mq_send (driver)");
    }
}

void start_timer(unsigned int seconds) {
    struct sigevent sev = {
        .sigev_notify = SIGEV_THREAD,
        .sigev_notify_function = timer_callback,
    };
    struct itimerspec its = {
        .it_value = { .tv_sec = seconds, .tv_nsec = 0 },
    };

    current_status = BUSY;

    timer_create(CLOCK_REALTIME, &sev, &timer_id);
    timer_settime(timer_id, 0, &its, NULL);
}

void send_init_msg()
{
    char init_msg[64];
    snprintf(init_msg, sizeof(init_msg), "Driver %d created", getpid());
    if (mq_send(cli_queue, init_msg, sizeof(init_msg), 0) == -1) {
        perror("mq_send (driver)");
    }
}

void send_status() {
    char status_msg[64];
    struct itimerspec remaining;

    switch (current_status) {
        case AVAILABLE:
            snprintf(status_msg, sizeof(status_msg), "Driver: %d Status: AVAILABLE", getpid());
            break;
        case BUSY:
            timer_gettime(timer_id, &remaining);
            snprintf(status_msg, sizeof(status_msg), "Driver: %d Status: BUSY %ld sec", getpid(), remaining.it_value.tv_sec);
            break;
        default:
            return;
    }

    if (mq_send(cli_queue, status_msg, sizeof(status_msg), 0) == -1) {
        perror("mq_send (driver)");
    }
}

void do_task(int time_sec) {
    char task_msg[64];
    struct itimerspec remaining;

    if (current_status == BUSY) {
        timer_gettime(timer_id, &remaining);
        snprintf(task_msg, sizeof(task_msg), "Driver: %d Status: BUSY %ld sec", getpid(), remaining.it_value.tv_sec);

    }
    else {
        snprintf(task_msg, sizeof(task_msg), "Driver %d start doing task", getpid());
        start_timer(time_sec);
    }

    if (mq_send(cli_queue, task_msg, sizeof(task_msg), 0) == -1) {
        perror("mq_send (driver)");
    }
}

void request_handler(const char *request) {  
    int time_sec;

    if (strcmp(request, "exit") == 0) {
        close(epoll_base);
        mq_close(my_queue);
        mq_close(cli_queue);
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(request, "status") == 0) {
        send_status();
    }
    else if (strncmp(request, "task", 4) == 0) {
        if (sscanf(request, "%*s %d", &time_sec) == 1) {
            do_task(time_sec);
        }
    }
}
