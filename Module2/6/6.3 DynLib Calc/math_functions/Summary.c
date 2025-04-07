#include <stdio.h>
#include <stdlib.h>

//  сумма 
double* summary(double num1, double num2, double* result) {
    *result = num1 + num2;
    return result;
}