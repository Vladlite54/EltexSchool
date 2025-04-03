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

typedef double* (*operation)(double, double, double*);

// Вызов основного меню калькулятора
void calculatorMenu() {

    void* handleSum; 
    void* handleSub; 
    void* handleMult; 
    void* handleDevide;
    operation summary, substract, multiply, devide;

    handleSum = dlopen("/home/vladlite/EltexSchool/Module2/6/6.3 DynLib Calc/math_functions/libsumm.so", RTLD_LAZY);
    handleSub = dlopen("/home/vladlite/EltexSchool/Module2/6/6.3 DynLib Calc/math_functions/libsubstr.so", RTLD_LAZY);
    handleMult = dlopen("/home/vladlite/EltexSchool/Module2/6/6.3 DynLib Calc/math_functions/libmulti.so", RTLD_LAZY);
    handleDevide = dlopen("/home/vladlite/EltexSchool/Module2/6/6.3 DynLib Calc/math_functions/libdevide.so", RTLD_LAZY);

    if (!handleSum || !handleSub || !handleMult || !handleDevide) {
        fprintf(stderr, "Error of open: %s\n", dlerror());
        exit(1);
    }

    summary = (operation) dlsym(handleSum, "summary");
    substract = (operation) dlsym(handleSub, "substract");
    multiply = (operation) dlsym(handleMult, "multiply");
    devide = (operation) dlsym(handleDevide ,"devide");


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
    double* (*math)(double, double, double*) = NULL;

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
                math = summary;
                break;
            case 2:
                math = substract;
                break;
            case 3:
                math = multiply;
                break;
            case 4:
                math = devide;
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
            if (math(num1, num2, &result) == NULL) printf("Can not devide by 0!\n");
            else printf("Result: %lf\n", result);
        }
    }

    dlclose(handleSum);
    dlclose(handleSub);
    dlclose(handleMult);
    dlclose(handleDevide);
}
