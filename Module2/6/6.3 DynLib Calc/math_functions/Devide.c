#include <stdio.h>
#include <stdlib.h>

//  деление
double* devide(double num1, double num2, double* result) {
    if (num2 == 0) return NULL;
    *result = num1 / num2;
    return result;
}