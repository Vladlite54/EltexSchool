#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

double summary(int count, ...);   // сложение
double substract(double num1, double num2); // вычитание
double multiply(double num1, double num2);  // умножение
double* devide(double num1, double num2, double* result);    // деление