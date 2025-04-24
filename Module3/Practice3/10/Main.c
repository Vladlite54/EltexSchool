#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <string.h>

#define SEM_NAME "/file_access_sem"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        perror("To low args\n");
        exit(EXIT_FAILURE);
    }

    int numOfNubers = atoi(argv[1]);
    char* filename = argv[2];

    pid_t pid;
    int pipefd[2];
    sem_t *sem;
    srand(time(NULL));

    sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open\n");
        exit(EXIT_FAILURE);
    }

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

        for (int i = 0; i < numOfNubers; i++) {
            int number = rand() % 100;
            if (write(pipefd[1], &number, sizeof(number)) != sizeof(number)) {
                perror("Write error\n");
                exit(EXIT_FAILURE);
            }
            printf("Send: %d\n", number);
            usleep(1000);

            printf("Waiting signal\n");
            
            if (sem_wait(sem) == -1) {
                perror("sem_wait\n");
                exit(EXIT_FAILURE);
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

            if (sem_post(sem) == -1) {
                perror("sem_post\n");
                exit(EXIT_FAILURE);
            }

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

            printf("Received: %d\n", number);

            if (sem_wait(sem) == -1) {
                perror("sem_wait\n");
                exit(EXIT_FAILURE);
            }

            FILE* file = fopen(filename, "a");
            if (file == NULL) {
                perror("File open error\n");
                exit(EXIT_FAILURE);
            }
            fprintf(file, "%d\n", number);
            fclose(file);

            if (sem_post(sem) == -1) {
                perror("sem_post\n");
                exit(EXIT_FAILURE);
            }

            usleep(1000);
        }
        close(pipefd[0]);
        wait(NULL);
        sem_close(sem);
        sem_unlink(SEM_NAME);
        printf("End %d\n", getpid());
    }

    return 0;
}