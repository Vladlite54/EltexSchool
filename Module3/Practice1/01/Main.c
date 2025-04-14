#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    
    int sides[argc];

    for (int i = 1; i < argc; ++i) {
        sides[i] = atoi(argv[i]);
    }

    int rv;
    pid_t pid;

    switch (pid = fork()) {
        case -1:
            perror("fork"); /* произошла ошибка */
            exit(1); /*выход из родительского процесса*/
            break;
        case 0:
            printf(" CHILD: Это процесс-потомок!\n");
            printf(" CHILD: Мой PID -- %d\n", getpid());
            printf(" CHILD: PID моего родителя -- %d\n", getppid());
            for (int i = 1; i < argc; ++i) {
                if (sides[i] % 2 == 0) {
                    int square = sides[i] * sides[i];
                    printf("Square: %d\n", square);
                }
            }
            exit(EXIT_SUCCESS);
            break;
        default:
            printf("PARENT: Это процесс-родитель!\n");
            printf("PARENT: Мой PID -- %d\n", getpid());
            for (int i = 1; i < argc; ++i) {
                if (sides[i] % 2 != 0) {
                    int square = sides[i] * sides[i];
                    printf("Square: %d\n", square);
                }
            }
            wait(&rv);
            printf("PARENT: RETURN STATUS FOR CHILD- %d\n", WEXITSTATUS(rv));
            break;
    }
    
    exit(EXIT_SUCCESS);
}