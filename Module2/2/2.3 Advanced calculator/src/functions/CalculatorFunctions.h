#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

double* summary(double num1, double num2, double* result);   // сложение
double* substract(double num1, double num2, double* result); // вычитание
double* multiply(double num1, double num2, double* result);  // умножение
double* devide(double num1, double num2, double* result);    // деление
double* (*selectOperation(int choice))(double, double, double*); // выбор нужной функции
