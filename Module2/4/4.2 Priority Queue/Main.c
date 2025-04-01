#include <stdio.h>
#include <stdbool.h>
#include "Queue.h"

int get_int() {
    int number;
    char t;
    while (scanf("%d%c", &number, &t) != 2 || t != '\n') {
        printf("Wrong format!\n");
        while (getchar() != '\n') {}
    }
    return number;
}

int main() {
    const char* info =
    "Command list:\n"
    "-> -1: Close program\n"
    "-> 0: Show command list\n"
    "-> 1: Push\n"
    "-> 2: Pop\n"
    "-> 3: Top\n"
    "-> 4: Show\n"
    "-> 5: Show higher or equals than priority\n"
    "-> 6: Top by priority\n"
    "-> 7: Pop by priority\n";

    bool flag = true;
    int input, id;
    Package newPack;
    Package* firstOut = NULL;
    char newData[100];
    int newPrior;
    PriorityQueue* q = makeQueue();

    printf("%s", info);
    while (flag) {
        printf("Enter command -> ");
        input = get_int();
        switch(input){
            case -1:
                flag = false;
                break;
            case 0:
                printf("%s", info);
                break;
            case 1:
                printf("Enter data: ");
                fgets(newData, sizeof(newData), stdin);
                newData[strlen(newData) - 1] = '\0';
                if (strlen(newData) == 0) {
                    printf("Must be filled in!\n");
                    break;
                }
                printf("Enter priority: ");
                newPrior = get_int();
                newPack = makePackage(newData, newPrior);
                push(q, newPack);
                printf("Package has been added!\n");
                break;
            case 2:
                if (pop(q) == -1) printf("Nothing to pop!\n");
                else printf("Package has been deleted\n");
                break;
            case 3:
                firstOut = top(q);
                if (firstOut != NULL) printf("Top-> Priority: %d Data: %s\n", firstOut->priority, firstOut->data);
                else printf("Nothing\n");
                break;
            case 4:
                printf("-----Packages-----\n");
                for (Node* current = q->head; current != NULL; current = current->next) {
                    printf("Priority: %d Data: %s\n", current->value.priority, current->value.data);
                }
                break;
            case 5:
                printf("Enter priority -> ");
                input = get_int();
                if (q->size == 0) break;
                for (Node* current = q->head; current->value.priority <= input; current = current->next) {
                    printf("Priority: %d Data: %s\n", current->value.priority, current->value.data);
                    if (current->next == NULL) break;
                }
                break;
            case 6:
                printf("Enter priority -> ");
                input = get_int();
                firstOut = topByPriority(q, input);
                if (firstOut != NULL ) printf("Top-> Priority: %d Data: %s\n", firstOut->priority, firstOut->data);
                else printf("Nothing\n");
                break;
            case 7:
                printf("Enter priority -> ");
                input = get_int();
                if (popByPriority(q, input) == -1) printf("Nothing to pop!\n");
                else printf("Package has been deleted\n");
                break;
            default:
                printf("Unknown command!\n");
                break;
        }
    }
    return 0;
}