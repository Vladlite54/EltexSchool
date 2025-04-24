#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

struct sembuf lock = {0, -1, SEM_UNDO};  // Блокировка
struct sembuf unlock = {0, 1, SEM_UNDO}; // Разблокировка

int main(int argc, char *argv[]) {
    if (argc < 3) {
        perror("To low args\n");
        exit(EXIT_FAILURE);
    }

    int numOfNubers = atoi(argv[1]);
    char* filename = argv[2];

    pid_t pid;
    int pipefd[2];
    int semid;
    key_t key;
    srand(time(NULL));

    key = ftok(".", 'S');
    if (key == -1) {
        perror("ftok err\n");
        exit(EXIT_FAILURE);
    }

    semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget err\n");
        exit(EXIT_FAILURE);
    }

    if (semctl(semid, 0, SETVAL, 1) == -1) {
        perror("semctl err\n");
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
            
            if (semop(semid, &lock, 1) == -1) {
                perror("semop lock err\n");
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

            if (semop(semid, &unlock, 1) == -1) {
                perror("semop unlock err\n");
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

            if (semop(semid, &lock, 1) == -1) {
                perror("semop lock err\n");
                exit(EXIT_FAILURE);
            }

            FILE* file = fopen(filename, "a");
            if (file == NULL) {
                perror("File open error\n");
                exit(EXIT_FAILURE);
            }
            fprintf(file, "%d\n", number);
            fclose(file);

            if (semop(semid, &unlock, 1) == -1) {
                perror("semop unlock err\n");
                exit(EXIT_FAILURE);
            }

            usleep(1000);
        }
        close(pipefd[0]);
        wait(NULL);

        if (semctl(semid, 0, IPC_RMID) == -1) {
            perror("semctl IPC_RMID err\n");
            exit(EXIT_FAILURE);
        }

        printf("End %d\n", getpid());
    }

    return 0;
}