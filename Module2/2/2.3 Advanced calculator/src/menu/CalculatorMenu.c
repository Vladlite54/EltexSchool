#include "CalculatorMenu.h"

// Ввод целого с клавиатуры
int get_int() {
    int number;
    char t;
    while (scanf("%d%c", &number, &t) != 2 || t != '\n') {
        printf("Wrong format!\n");
        while (getchar() != '\n') {}
    }
    return number;
}

//  Корректный ввод вещественного
double get_double() {
    char answer[256];
    double x;
    fgets(answer, sizeof(answer), stdin);
    while (sscanf(answer, "%lf", &x) != 1) {
        printf("Wrong format!\n");
        fgets(answer, sizeof(answer), stdin);
    }
    return x;
}

// Вызов основного меню калькулятора
void calculatorMenu() {
    printf("-----Calculator-----");
    const char* info =
    "Operation list:\n"
    "-> -1: Close program\n"
    "-> 0: Show operation list\n"
    "-> 1: Sum\n"
    "-> 2: Substract\n"
    "-> 3: Multiply\n"
    "-> 4: Devision\n";

    bool flag = true;
    int input, argsNumber;
    double num1, num2, result;
    double* (*operation)(double, double, double*) = NULL;

    printf("%s", info);
    while (flag) {
        bool isMathOperation = true;
        printf("Enter command -> ");
        input = get_int();
        switch (input) {
            case -1:
                flag = false;
                isMathOperation = false;
                break;
            case 0:
                printf("%s", info);
                isMathOperation = false;
                break;
            case 1:
                operation = selectOperation(1);
                break;
            case 2:
                operation = selectOperation(2);
                break;
            case 3:
                operation = selectOperation(3);
                break;
            case 4:
                operation = selectOperation(4);
                break;
            default:
                isMathOperation = false;
                printf("Unknown command!\n");
                break;
               
        }
        if (isMathOperation) {
            printf("Enter arg1 -> ");
            num1 = get_double();
            printf("Enter arg2 -> ");
            num2 = get_double();
            if (operation(num1, num2, &result) == NULL) printf("Can not devide by 0!\n");
            else printf("Result: %lf\n", result);
        }
    }

}
