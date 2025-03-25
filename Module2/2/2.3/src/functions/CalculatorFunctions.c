#include "CalculatorFunctions.h"

//  сумма 
double* summary(double num1, double num2, double* result) {
    *result = num1 + num2;
    return result;
}

//  разность
double* substract(double num1, double num2, double* result) {
    *result = num1 - num2;
    return result;
}

// умножение
double* multiply(double num1, double num2, double* result) {
    *result = num1 * num2;
    return result;
}

//  деление
double* devide(double num1, double num2, double* result) {
    if (num2 == 0) return NULL;
    *result = num1 / num2;
    return result;
}

// выбор нужной функции
double *(*selectOperation(int choice))(double, double, double *) {
    switch (choice) {
        case 1: return summary;
        case 2: return substract;
        case 3: return multiply;
        case 4: return devide;
        default: return summary;
    }
}
