#include "CalculatorFunctions.h"

//  сложение 
double summary(int count, ...) {
    double result = 0;
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; ++i) {
        result += va_arg(args, double);
    }
    va_end(args);
    return result;
}

//  вычитание
double substract(double num1, double num2) {
    return num1 - num2;
}

// умножение
double multiply(double num1, double num2) {
    return num1 * num2;
}

double* devide(double num1, double num2, double* result) {
    if (num2 == 0) return NULL;
    *result = num1 / num2;
    return result;
}
