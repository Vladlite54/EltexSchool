#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <time.h>
#include <signal.h>

#define MAX_NUMBERS 100

typedef struct SharedData {
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

    key_t key = ftok(".", 'S');
    int shm_id = shmget(key, sizeof(SharedData), IPC_CREAT | 0666);
    SharedData* shared = (SharedData*)shmat(shm_id, NULL, 0);

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
            shmdt(shared);
            exit(0);
        } else {
            wait(NULL);
            printf("Set %d: Min = %d, Max = %d\n", shared->processed_sets + 1, shared->min, shared->max);
            shared->processed_sets++;
            sleep(1); // Задержка для наглядности
        }
    }

    printf("\nTotal processed sets: %d\n", shared->processed_sets);

    shmdt(shared);
    shmctl(shm_id, IPC_RMID, NULL);
    return 0;
}