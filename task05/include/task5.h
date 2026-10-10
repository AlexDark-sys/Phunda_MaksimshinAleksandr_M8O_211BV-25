#ifndef TASK5_H
#define TASK5_H

typedef enum {
    OK,
    INVALID_ARGUMENT,
    DIVERGES,
    PRECISION_ERROR,
    ITERATION_LIMIT
} Status;

typedef Status (*Function)(long double, long double *);

Status parse_number(const char *text, long double *result);

Status sum_series(int kind, long double x,
                  long double eps, long double *result);

Status function_a(long double x, long double *result);
Status function_b(long double x, long double *result);
Status function_c(long double x, long double *result);
Status function_d(long double x, long double *result);

Status integrate(Function function, long double eps,
                 long double *result);

#endif