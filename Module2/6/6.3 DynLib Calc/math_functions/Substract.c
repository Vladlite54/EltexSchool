#include <stdio.h>
#include <stdlib.h>

//  ��������
double* substract(double num1, double num2, double* result) {
    *result = num1 - num2;
    return result;
}