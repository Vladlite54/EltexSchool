#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>

#define MAX_ARGS 64

void commandHandler(char** args) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("Error!\n");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) {
        execvp(args[0], args);
    } 
    else {
        wait(NULL);
    }
}

int main(int argc, char* argv[]) {

    int a = 0;
    char input[100];
    char* args[MAX_ARGS];
    char* token;

    while (true) {
        printf(">> ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Ошибка чтения ввода");
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        int agrsCount = 0;
        token = strtok(input, " ");

        while (token != NULL && agrsCount < MAX_ARGS - 1) {
            args[agrsCount++] = token;
            token = strtok(NULL, " ");
        }

        args[agrsCount] = NULL;

        if (agrsCount == 0) continue;

        if (strcmp(args[0], "exit") == 0) break;

        commandHandler(args);
    }
        
    exit(EXIT_SUCCESS);
}