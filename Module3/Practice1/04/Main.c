#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char* argv[]) {
    int rv;
    pid_t pid;
    int pipefd[2];
    if(pipe(pipefd) == -1) {
        perror("Pipe error\n");
        exit(EXIT_FAILURE);
    };

    switch (pid = fork()) {
        case -1:
            perror("Error\n");
            exit(EXIT_FAILURE);
            break;
        case 0:
            printf(" CHILD: Это процесс-потомок!\n");
            printf(" CHILD: Мой PID -- %d\n", getpid());
            printf(" CHILD: PID моего родителя -- %d\n", getppid());
            close(pipefd[0]);
            srand(time(NULL));
            for (int i = 0; i < atoi(argv[1]); ++i) {
                int number = rand() % 100;
                if (write(pipefd[1], &number, sizeof(number)) != sizeof(number)) {
                    perror("Write error\n");
                    exit(EXIT_FAILURE);
                }
                printf("written\n");
            }
            close(pipefd[1]);
            exit(EXIT_SUCCESS);
            break;
        default:
            printf("PARENT: Это процесс-родитель!\n");
            printf("PARENT: Мой PID -- %d\n", getpid());
            close(pipefd[1]);
            wait(&rv);
            printf("PARENT: RETURN STATUS FOR CHILD- %d\n", WEXITSTATUS(rv));
            for (int i = 0; i < atoi(argv[1]); ++i) {
                int number;
                if (read(pipefd[0], &number, sizeof(number)) != sizeof(number)) {
                    perror("Read error\n");
                    exit(EXIT_FAILURE);
                }
                printf("%d\n", number);
            }
            close(pipefd[0]);
            break;
    }
    
    exit(EXIT_SUCCESS);
}