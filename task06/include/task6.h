#ifndef TASK6_H
#define TASK6_H

#include <stddef.h>

typedef enum {
    STATUS_OK = 0,
    STATUS_INVALID_ARGUMENT,
    STATUS_INVALID_NUMBER,
    STATUS_OVERFLOW,
    STATUS_MEMORY_ERROR,
    STATUS_NO_ROOT,
    STATUS_PRECISION_ERROR,
    STATUS_ITERATION_LIMIT
} Status;

typedef Status (*Equation)(double x, double *result);

Status is_convex(double eps, int *result, int count, ...);

Status polynomial(double x, int degree, double *result, ...);

Status kaprekar_numbers(int base, unsigned long long **result,
                        size_t *result_count, int count, ...);

Status geometric_mean(double *result, int count, ...);

Status fast_power(double x, int exponent, double eps,
                  double *result);

Status bisection(double left, double right, double eps,
                 Equation function, double *result);

#endif