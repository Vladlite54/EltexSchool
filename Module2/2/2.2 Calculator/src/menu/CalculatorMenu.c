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
    char buff[50];
    double* args = NULL;

    printf("%s", info);
    while (flag) {
        printf("Enter command -> ");
        input = get_int();
        switch (input) {
            case -1:
                flag = false;
                break;
            case 0:
                printf("%s", info);
                break;
            case 1:
                printf("Enter number of agrs (from 2 to 4) -> ");
                argsNumber = get_int();
                if (argsNumber < 2 || argsNumber > 4) {
                    printf("Incorrect args number!\n");
                    break;
                }
                args = (double*)malloc(sizeof(double) * argsNumber);
                printf("Args ->\n");
                for (int i = 0; i < argsNumber; ++i) {
                    args[i] = get_double();
                }
                switch (argsNumber) {
                case 2:
                    result = summary(2, args[0], args[1]);
                    printf("Result: %lf\n", result);
                    break;
                case 3:
                    result = summary(3, args[0], args[1], args[2]);
                    printf("Result: %lf\n", result);
                    break;
                case 4:
                    result = summary(4, args[0], args[1], args[2], args[3]);
                    printf("Result: %lf\n", result);
                    break;
                default:
                    printf("Incorrect arg number!\n");
                    break;
                }
                break;
            case 2:
                printf("Enter args (Format: arg1 arg2):\n");
                if (scanf("%lf %lf", &num1, &num2) != 2) {
                    printf("Incorret format!\n");
                    break;
                }
                result = substract(num1, num2);
                printf("Result: %lf\n", result);
                break;
            case 3:
                printf("Enter args (Format: arg1 arg2):\n");
                if (scanf("%lf %lf", &num1, &num2) != 2) {
                    printf("Incorret format!\n");
                    break;
                }
                result = multiply(num1, num2);
                printf("Result: %lf\n", result);
                break;
            case 4:
                printf("Enter args (Format: arg1 arg2):\n");
                if (scanf("%lf %lf", &num1, &num2) != 2) {
                    printf("Incorret format!\n");
                    break;
                }
                if (devide(num1, num2, &result) == NULL) {
                    printf("Can not devide by 0!\n");
                }
                else {
                    printf("Result: %lf\n", result);
                }
                break;
            default:
                printf("Unknown command!\n");
                break;
                
        }
    }

}
