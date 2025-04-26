#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

#define MAX_READERS 3
#define READER_SEM 0
#define WRITER_SEM 1
#define MUTEX_SEM 2

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void sem_lock(int semid, int sem_num) {
    struct sembuf op = {sem_num, -1, SEM_UNDO};
    semop(semid, &op, 1);
}

void sem_unlock(int semid, int sem_num) {
    struct sembuf op = {sem_num, 1, SEM_UNDO};
    semop(semid, &op, 1);
}

void readFile(const char* filename) {
    FILE* read = fopen(filename, "r");
    if (read == NULL) return;
    printf("Reader %d read: ", getpid());
    char ch;
    while ((ch = fgetc(read)) != EOF) putchar(ch);
    fclose(read);
    //usleep(1000);
}

void writeToFile(const char* filename) {
    FILE* write= fopen(filename, "a");
    if (write == NULL) return;
    fprintf(write, "Note by %d\n", getpid());
    fclose(write);
    printf("Writer %d wrote to file\n", getpid());
    //usleep(1000);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        perror("To low args\n");
        exit(EXIT_FAILURE);
    }

    int processNum = atoi(argv[1]);
    char* filename = argv[2];

    pid_t pid;
    int semid;
    key_t key = ftok(".", 'A');
    union semun arg;
    int read_count = 0;

    int old_semid = semget(key, 3, 0666);
    if (old_semid != -1) {
        semctl(old_semid, 0, IPC_RMID);
    }

    semid = semget(key, 3, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    arg.val = MAX_READERS;
    semctl(semid, READER_SEM, SETVAL, arg);
    arg.val = 1;
    semctl(semid, WRITER_SEM, SETVAL, arg);
    semctl(semid, MUTEX_SEM, SETVAL, arg);

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("File open error\n");
        exit(1);
    }
    fclose(file);

    for (int i = 0; i < processNum; i++) {
        pid = fork();
        if (pid == 0) {
            if (i % 2 == 0) {
                sem_lock(semid, READER_SEM);

                sem_lock(semid, MUTEX_SEM);
                read_count++;
                if (read_count == 1) {
                    sem_lock(semid, WRITER_SEM);
                }
                sem_unlock(semid, MUTEX_SEM);

                readFile(filename);

                sem_lock(semid, MUTEX_SEM);
                read_count--;
                if (read_count == 0) {
                    sem_unlock(semid, WRITER_SEM);
                }
                sem_unlock(semid, MUTEX_SEM);
                sem_unlock(semid, READER_SEM);
            }
            else {
                sem_lock(semid, WRITER_SEM);
                writeToFile(filename);
                sem_unlock(semid, WRITER_SEM);
            }
            exit(0);
        }
    }
    
    while(wait(NULL) > 0);

    semctl(semid, 0, IPC_RMID);

    return 0;
}