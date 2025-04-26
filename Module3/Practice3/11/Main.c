#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <string.h>

#define MAX_READERS 3
#define SEM_READER_NAME "/reader_sem"
#define SEM_WRITER_NAME "/writer_sem"
#define SEM_MUTEX_NAME "/mutex_sem"

void readFile(const char* filename) {
    FILE* read = fopen(filename, "r");
    if (read == NULL) return;
    printf("Reader %d read: ", getpid());
    char ch;
    while ((ch = fgetc(read)) != EOF) putchar(ch);
    fclose(read);
}

void writeToFile(const char* filename) {
    FILE* write= fopen(filename, "a");
    if (write == NULL) return;
    fprintf(write, "Note by %d\n", getpid());
    fclose(write);
    printf("Writer %d wrote to file\n", getpid());
}

int main(int argc, char *argv[]) {

    sem_unlink(SEM_READER_NAME);
    sem_unlink(SEM_WRITER_NAME);
    sem_unlink(SEM_MUTEX_NAME);

    if (argc < 3) {
        perror("To low args\n");
        exit(EXIT_FAILURE);
    }

    int processNum = atoi(argv[1]);
    char* filename = argv[2];

    pid_t pid;
    sem_t *reader_sem, *writer_sem, *mutex;
    int read_count = 0;

    reader_sem = sem_open(SEM_READER_NAME, O_CREAT, 0644, MAX_READERS);
    writer_sem = sem_open(SEM_WRITER_NAME, O_CREAT, 0644, 1);
    mutex = sem_open(SEM_MUTEX_NAME, O_CREAT, 0644, 1);
    if (reader_sem == SEM_FAILED || writer_sem == SEM_FAILED || mutex == SEM_FAILED) {
        perror("Sem error\n");
        exit(EXIT_FAILURE);
    }
    
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
                sem_wait(reader_sem);

                sem_wait(mutex);
                read_count++;
                if (read_count == 1) {
                    sem_wait(writer_sem);
                }
                sem_post(mutex);

                readFile(filename);

                sem_wait(mutex);
                read_count--;
                if (read_count == 0) {
                    sem_post(writer_sem);
                }
                sem_post(mutex);
                sem_post(reader_sem);
            }
            else {
                sem_wait(writer_sem);
                writeToFile(filename);
                sem_post(writer_sem);
            }
            exit(0);
        }
    }
    
    while(wait(NULL) > 0);

    sem_close(reader_sem);
    sem_close(writer_sem);
    sem_close(mutex);
    sem_unlink(SEM_READER_NAME);
    sem_unlink(SEM_WRITER_NAME);
    sem_unlink(SEM_MUTEX_NAME);

    return 0;
}