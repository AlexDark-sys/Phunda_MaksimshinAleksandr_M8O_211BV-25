#include "task6.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static Status equation_square(double x, double *result)
{
    if (result == NULL || !isfinite(x)) {
        return STATUS_INVALID_ARGUMENT;
    }

    *result = x * x - 2.0;
    return isfinite(*result) ? STATUS_OK : STATUS_OVERFLOW;
}

static Status equation_cos(double x, double *result)
{
    if (result == NULL || !isfinite(x)) {
        return STATUS_INVALID_ARGUMENT;
    }

    *result = cos(x) - x;
    return isfinite(*result) ? STATUS_OK : STATUS_OVERFLOW;
}

static Status equation_cube(double x, double *result)
{
    if (result == NULL || !isfinite(x)) {
        return STATUS_INVALID_ARGUMENT;
    }

    *result = x * x * x - 2.0;
    return isfinite(*result) ? STATUS_OK : STATUS_OVERFLOW;
}

int main(int argc, char *argv[])
{
    (void)argv;

    if (argc != 1) {
        fprintf(stderr, "No command-line arguments expected.\n");
        return 1;
    }

    const double eps = 1e-9;
    Status status;

    /* 1. Выпуклый квадрат. */
    int convex;

    status = is_convex(
        eps, &convex, 4,
        0.0, 0.0,
        2.0, 0.0,
        2.0, 2.0,
        0.0, 2.0
    );

    if (status != STATUS_OK) {
        fprintf(stderr, "Polygon error: %d\n", (int)status);
        return 1;
    }

    if (printf("Square is convex: %s\n",
               convex ? "yes" : "no") < 0) {
        return 1;
    }

    /* Невыпуклый многоугольник. */
    status = is_convex(
        eps, &convex, 5,
        0.0, 0.0,
        2.0, 0.0,
        1.0, 0.5,
        2.0, 2.0,
        0.0, 2.0
    );

    if (status != STATUS_OK) {
        fprintf(stderr, "Polygon error: %d\n", (int)status);
        return 1;
    }

    if (printf("Second polygon is convex: %s\n",
               convex ? "yes" : "no") < 0) {
        return 1;
    }

    /* 2. P(x) = 2x^2 - 3x + 1, x = 2. */
    double value;

    status = polynomial(2.0, 2, &value, 2.0, -3.0, 1.0);

    if (status != STATUS_OK) {
        fprintf(stderr, "Polynomial error: %d\n", (int)status);
        return 1;
    }

    if (printf("\nP(2) = %.10g\n", value) < 0) {
        return 1;
    }

    /* 3. Числа Капрекара в основании 10. */
    unsigned long long *numbers = NULL;
    size_t count = 0;

    status = kaprekar_numbers(
        10, &numbers, &count, 6,
        (const char *)"1",
        (const char *)"9",
        (const char *)"45",
        (const char *)"55",
        (const char *)"99",
        (const char *)"10"
    );

    if (status != STATUS_OK) {
        fprintf(stderr, "Kaprekar error: %d\n", (int)status);
        free(numbers);
        return 1;
    }

    int output_failed = 0;

    if (printf("\nKaprekar numbers:") < 0) {
        output_failed = 1;
    }

    for (size_t i = 0; i < count && !output_failed; ++i) {
        if (printf(" %llu", numbers[i]) < 0) {
            output_failed = 1;
        }
    }

    free(numbers);

    if (output_failed || printf("\n") < 0) {
        return 1;
    }

    /* 4. Кубический корень из 1 * 4 * 16 равен 4. */
    status = geometric_mean(&value, 3, 1.0, 4.0, 16.0);

    if (status != STATUS_OK) {
        fprintf(stderr, "Geometric mean error: %d\n", (int)status);
        return 1;
    }

    if (printf("\nGeometric mean = %.10g\n", value) < 0) {
        return 1;
    }

    /* 5. Быстрое возведение в степень. */
    status = fast_power(2.0, -3, eps, &value);

    if (status != STATUS_OK) {
        fprintf(stderr, "Power error: %d\n", (int)status);
        return 1;
    }

    if (printf("\n2^(-3) = %.10g\n", value) < 0) {
        return 1;
    }

    /* 6. Разные уравнения, интервалы и точности. */
    status = bisection(1.0, 2.0, 1e-6,
                       equation_square, &value);

    if (status != STATUS_OK) {
        fprintf(stderr, "Bisection error: %d\n", (int)status);
        return 1;
    }

    if (printf("\nx^2 - 2 = 0: %.10g\n", value) < 0) {
        return 1;
    }

    status = bisection(0.0, 1.0, 1e-9,
                       equation_cos, &value);

    if (status != STATUS_OK) {
        fprintf(stderr, "Bisection error: %d\n", (int)status);
        return 1;
    }

    if (printf("cos(x) - x = 0: %.10g\n", value) < 0) {
        return 1;
    }

    status = bisection(0.0, 3.0, 1e-7,
                       equation_cube, &value);

    if (status != STATUS_OK) {
        fprintf(stderr, "Bisection error: %d\n", (int)status);
        return 1;
    }

    if (printf("x^3 - 2 = 0: %.10g\n", value) < 0 ||
        fflush(stdout) == EOF) {
        return 1;
    }

    return 0;
}