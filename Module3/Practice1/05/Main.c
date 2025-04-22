#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

volatile sig_atomic_t file_locked = 0;

void sigusr1Handler(int sig) {
    file_locked = 1;
}

void sigusr2Handler(int sig) {
    file_locked = 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        perror("To low args\n");
        exit(EXIT_FAILURE);
    }

    int numOfNubers = atoi(argv[1]);
    char* filename = argv[2];

    pid_t pid;
    int pipefd[2];
    srand(time(NULL));

    if (pipe(pipefd) == -1) {
        perror("Pipe error\n");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid == -1) {
        perror("Fork error\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        printf("Child: %d\n", getpid());
        close(pipefd[0]);
        signal(SIGUSR1, sigusr1Handler);
        signal(SIGUSR2, sigusr2Handler);

        for (int i = 0; i < numOfNubers; i++) {
            int number = rand() % 100;
            if (write(pipefd[1], &number, sizeof(number)) != sizeof(number)) {
                perror("Write error\n");
                exit(EXIT_FAILURE);
            }
            printf("Send: %d\n", number);
            usleep(1000);

            printf("Waiting signal\n");
            while (file_locked) {
                pause();
            }

            FILE* file = fopen(filename, "r");
            if (file == NULL) {
                perror("File open error\n");
                exit(EXIT_FAILURE);
            }
            char buff[50];
            printf("File: ");
            while (fgets(buff, sizeof(buff), file) != NULL) {
                buff[strlen(buff) - 1] = '\0';
                printf("%s ", buff);
            }
            printf("\n");
            fclose(file);
            usleep(1000);
        }

        close(pipefd[1]);
        exit(EXIT_SUCCESS);
    }
    else {
        printf("Parent: %d\n", getpid());
        close(pipefd[1]);
        FILE* file = fopen(filename, "w");
        if (file == NULL) {
            perror("File open error\n");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < numOfNubers; i++) {
            int number = 0;
            if (read(pipefd[0], &number, sizeof(number)) != sizeof(number)) {
                perror("Read error\n");
                exit(EXIT_FAILURE);
            }

            kill(pid, SIGUSR1);
            FILE* file = fopen(filename, "a");
            if (file == NULL) {
                perror("File open error\n");
                exit(EXIT_FAILURE);
            }
            fprintf(file, "%d\n", number);
            fclose(file);
            kill(pid, SIGUSR2);

            printf("Received: %d\n", number);
            usleep(1000);
        }
        close(pipefd[0]);
        wait(NULL);
        printf("End %d\n", getpid());
    }

    return 0;
}