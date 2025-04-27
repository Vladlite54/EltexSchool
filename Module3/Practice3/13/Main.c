#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <string.h>

#define SHM_NAME "/shared_mem_posix"
#define MAX_NUMBERS 100

typedef struct {
    int numbers[MAX_NUMBERS];
    int count;
    int min;
    int max;
    int processed_sets;
} SharedData;

volatile sig_atomic_t running = 1;

void handle_sigint(int sig) {
    running = 0;
}

int main() {
    signal(SIGINT, handle_sigint);
    srand(time(NULL));

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedData));
    SharedData* shared = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    shared->processed_sets = 0;

    while (running) {
        shared->count = rand() % MAX_NUMBERS + 1;
        for (int i = 0; i < shared->count; i++) {
            shared->numbers[i] = rand() % 1000;
        }

        pid_t pid = fork();
        if (pid == 0) {
            shared->min = shared->numbers[0];
            shared->max = shared->numbers[0];
            for (int i = 1; i < shared->count; i++) {
                if (shared->numbers[i] < shared->min) shared->min = shared->numbers[i];
                if (shared->numbers[i] > shared->max) shared->max = shared->numbers[i];
            }
            exit(0);
        } else {
            wait(NULL);
            printf("Set %d: Min = %d, Max = %d\n", shared->processed_sets + 1, shared->min, shared->max);
            shared->processed_sets++;
            sleep(1); // Задержка для наглядности
        }
    }

    printf("\nTotal processed sets: %d\n", shared->processed_sets);

    munmap(shared, sizeof(SharedData));
    shm_unlink(SHM_NAME);
    return 0;
}