#include "task6.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>

/* Тип, а не глобальная переменная. */
typedef struct {
    double x;
    double y;
} Point;

/* Знак ориентированной площади треугольника. */
static Status orientation(Point a, Point b, Point c,
                          double eps, int *result)
{
    if (result == NULL || !isfinite(eps) || eps <= 0.0) {
        return STATUS_INVALID_ARGUMENT;
    }

    long double value =
        ((long double)b.x - a.x) * ((long double)c.y - a.y) -
        ((long double)b.y - a.y) * ((long double)c.x - a.x);

    if (!isfinite(value)) {
        return STATUS_OVERFLOW;
    }

    *result = fabsl(value) <= eps ? 0 : (value > 0 ? 1 : -1);
    return STATUS_OK;
}

static int on_segment(Point a, Point b, Point p, double eps)
{
    return (long double)p.x >= fminl(a.x, b.x) - eps &&
           (long double)p.x <= fmaxl(a.x, b.x) + eps &&
           (long double)p.y >= fminl(a.y, b.y) - eps &&
           (long double)p.y <= fmaxl(a.y, b.y) + eps;
}

static Status segments_intersect(Point a, Point b,
                                 Point c, Point d,
                                 double eps, int *result)
{
    if (result == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    int s1, s2, s3, s4;

    Status status = orientation(a, b, c, eps, &s1);
    if (status != STATUS_OK) {
        return status;
    }

    status = orientation(a, b, d, eps, &s2);
    if (status != STATUS_OK) {
        return status;
    }

    status = orientation(c, d, a, eps, &s3);
    if (status != STATUS_OK) {
        return status;
    }

    status = orientation(c, d, b, eps, &s4);
    if (status != STATUS_OK) {
        return status;
    }

    *result =
        (s1 * s2 < 0 && s3 * s4 < 0) ||
        (s1 == 0 && on_segment(a, b, c, eps)) ||
        (s2 == 0 && on_segment(a, b, d, eps)) ||
        (s3 == 0 && on_segment(c, d, a, eps)) ||
        (s4 == 0 && on_segment(c, d, b, eps));

    return STATUS_OK;
}

Status is_convex(double eps, int *result, int count, ...)
{
    if (result == NULL || count < 3 ||
        !isfinite(eps) || eps <= 0.0) {
        return STATUS_INVALID_ARGUMENT;
    }

    *result = 0;
    size_t n = (size_t)count;

    if (n > SIZE_MAX / sizeof(Point)) {
        return STATUS_OVERFLOW;
    }

    Point *points = malloc(n * sizeof(*points));

    if (points == NULL) {
        return STATUS_MEMORY_ERROR;
    }

    Status status = STATUS_OK;
    va_list args;
    va_start(args, count);

    for (size_t i = 0; i < n; ++i) {
        points[i].x = va_arg(args, double);
        points[i].y = va_arg(args, double);

        if (!isfinite(points[i].x) || !isfinite(points[i].y)) {
            status = STATUS_INVALID_ARGUMENT;
            break;
        }
    }

    va_end(args);

    /* Одинаковые вершины не допускаются. */
    for (size_t i = 0; i < n && status == STATUS_OK; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (fabsl((long double)points[i].x - points[j].x) <= eps &&
                fabsl((long double)points[i].y - points[j].y) <= eps) {
                status = STATUS_INVALID_ARGUMENT;
                break;
            }
        }
    }

    /* Проверяем пересечения несоседних рёбер. */
    for (size_t i = 0; i < n && status == STATUS_OK; ++i) {
        size_t next_i = (i + 1) % n;

        for (size_t j = i + 1; j < n; ++j) {
            size_t next_j = (j + 1) % n;

            if (next_i == j || next_j == i) {
                continue;
            }

            int intersects;
            status = segments_intersect(
                points[i], points[next_i],
                points[j], points[next_j],
                eps, &intersects
            );

            if (status != STATUS_OK) {
                break;
            }

            if (intersects) {
                status = STATUS_INVALID_ARGUMENT;
                break;
            }
        }
    }

    int direction = 0;
    int convex = 1;

    for (size_t i = 0; i < n && status == STATUS_OK; ++i) {
        Point a = points[i];
        Point b = points[(i + 1) % n];
        Point c = points[(i + 2) % n];

        int sign;
        status = orientation(a, b, c, eps, &sign);

        if (status != STATUS_OK) {
            break;
        }

        if (sign == 0) {
            /*
             * На одной прямой допускается продолжение ребра,
             * но не движение обратно по нему.
             */
            long double dot =
                ((long double)b.x - a.x) *
                ((long double)c.x - b.x) +
                ((long double)b.y - a.y) *
                ((long double)c.y - b.y);

            if (!isfinite(dot)) {
                status = STATUS_OVERFLOW;
            } else if (dot < -eps) {
                status = STATUS_INVALID_ARGUMENT;
            }

            continue;
        }

        if (direction == 0) {
            direction = sign;
        } else if (sign != direction) {
            convex = 0;
        }
    }

    if (status == STATUS_OK) {
        if (direction == 0) {
            status = STATUS_INVALID_ARGUMENT;
        } else {
            *result = convex;
        }
    }

    free(points);
    return status;
}

Status polynomial(double x, int degree, double *result, ...)
{
    if (result == NULL || degree < 0 || !isfinite(x)) {
        return STATUS_INVALID_ARGUMENT;
    }

    va_list args;
    va_start(args, result);

    double value = va_arg(args, double);
    Status status = isfinite(value)
        ? STATUS_OK
        : STATUS_INVALID_ARGUMENT;

    /* Схема Горнера. */
    for (int i = 0; i < degree && status == STATUS_OK; ++i) {
        double coefficient = va_arg(args, double);

        if (!isfinite(coefficient)) {
            status = STATUS_INVALID_ARGUMENT;
            break;
        }

        value = value * x + coefficient;

        if (!isfinite(value)) {
            status = STATUS_OVERFLOW;
        }
    }

    va_end(args);

    if (status == STATUS_OK) {
        *result = value;
    }

    return status;
}

static Status parse_unsigned(const char *text, int base,
                             unsigned long long *result)
{
    if (text == NULL || result == NULL || base < 2 || base > 36) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (*text == '\0') {
        return STATUS_INVALID_NUMBER;
    }

    unsigned long long value = 0;

    for (size_t i = 0; text[i] != '\0'; ++i) {
        unsigned int digit;

        if (text[i] >= '0' && text[i] <= '9') {
            digit = (unsigned int)(text[i] - '0');
        } else if (text[i] >= 'A' && text[i] <= 'Z') {
            digit = (unsigned int)(text[i] - 'A' + 10);
        } else if (text[i] >= 'a' && text[i] <= 'z') {
            digit = (unsigned int)(text[i] - 'a' + 10);
        } else {
            return STATUS_INVALID_NUMBER;
        }

        if (digit >= (unsigned int)base) {
            return STATUS_INVALID_NUMBER;
        }

        if (value > (ULLONG_MAX - digit) / (unsigned int)base) {
            return STATUS_OVERFLOW;
        }

        value = value * (unsigned int)base + digit;
    }

    *result = value;
    return STATUS_OK;
}

static Status check_kaprekar(unsigned long long number,
                            int base, int *result)
{
    if (result == NULL || base < 2 || base > 36) {
        return STATUS_INVALID_ARGUMENT;
    }

    *result = 0;

    /* В этой реализации 0 не считается числом Капрекара. */
    if (number == 0) {
        return STATUS_OK;
    }

    if (number > ULLONG_MAX / number) {
        return STATUS_OVERFLOW;
    }

    unsigned long long square = number * number;
    unsigned long long divisor = (unsigned int)base;

    /*
     * Проверяем разделения квадрата на левую и правую части.
     * Правая часть должна быть ненулевой.
     */
    while (1) {
        unsigned long long left = square / divisor;
        unsigned long long right = square % divisor;

        if (right != 0 && left <= number &&
            right == number - left) {
            *result = 1;
            return STATUS_OK;
        }

        if (divisor > square) {
            break;
        }

        if (divisor > ULLONG_MAX / (unsigned int)base) {
            break;
        }

        divisor *= (unsigned int)base;
    }

    return STATUS_OK;
}

Status kaprekar_numbers(int base, unsigned long long **result,
                        size_t *result_count, int count, ...)
{
    if (result == NULL || result_count == NULL ||
        base < 2 || base > 36 || count <= 0) {
        return STATUS_INVALID_ARGUMENT;
    }

    *result = NULL;
    *result_count = 0;

    if ((size_t)count > SIZE_MAX / sizeof(unsigned long long)) {
        return STATUS_OVERFLOW;
    }

    unsigned long long *numbers =
        malloc((size_t)count * sizeof(*numbers));

    if (numbers == NULL) {
        return STATUS_MEMORY_ERROR;
    }

    size_t found = 0;
    Status status = STATUS_OK;

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; ++i) {
        const char *text = va_arg(args, const char *);
        unsigned long long number;

        status = parse_unsigned(text, base, &number);

        if (status != STATUS_OK) {
            break;
        }

        int matches;
        status = check_kaprekar(number, base, &matches);

        if (status != STATUS_OK) {
            break;
        }

        if (matches) {
            numbers[found++] = number;
        }
    }

    va_end(args);

    if (status != STATUS_OK) {
        free(numbers);
        return status;
    }

    if (found == 0) {
        free(numbers);
        return STATUS_OK;
    }

    *result = numbers;
    *result_count = found;

    return STATUS_OK;
}

Status geometric_mean(double *result, int count, ...)
{
    if (result == NULL || count <= 0) {
        return STATUS_INVALID_ARGUMENT;
    }

    long double mean_log = 0.0L;
    int has_zero = 0;
    Status status = STATUS_OK;

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; ++i) {
        double number = va_arg(args, double);

        /*
         * Проверки ниже задают область определения,
         * а не приближённое сравнение результатов.
         */
        if (!isfinite(number) || number < 0.0) {
            status = STATUS_INVALID_ARGUMENT;
            break;
        }

        if (number == 0.0) {
            has_zero = 1;
        } else {
            mean_log += logl(number) / (long double)count;
        }
    }

    va_end(args);

    if (status != STATUS_OK) {
        return status;
    }

    long double value = has_zero ? 0.0L : expl(mean_log);
    double converted = (double)value;

    if (!isfinite(value) || !isfinite(converted) ||
        (!has_zero && converted == 0.0)) {
        return STATUS_OVERFLOW;
    }

    *result = converted;
    return STATUS_OK;
}

/* exponent здесь неотрицательный. */
static Status power_recursive(double x, unsigned int exponent,
                              double *result)
{
    if (result == NULL || !isfinite(x)) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (exponent == 0) {
        *result = 1.0;
        return STATUS_OK;
    }

    double half;
    Status status = power_recursive(x, exponent / 2, &half);

    if (status != STATUS_OK) {
        return status;
    }

    double value = half * half;

    if (exponent % 2 != 0) {
        value *= x;
    }

    if (!isfinite(value) || (x != 0.0 && value == 0.0)) {
        return STATUS_OVERFLOW;
    }

    *result = value;
    return STATUS_OK;
}

Status fast_power(double x, int exponent, double eps,
                  double *result)
{
    if (result == NULL || !isfinite(x) ||
        !isfinite(eps) || eps <= 0.0) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (fabs(x) <= eps && exponent <= 0) {
        return STATUS_INVALID_ARGUMENT;
    }

    unsigned int magnitude = exponent < 0
        ? (unsigned int)(-(exponent + 1)) + 1U
        : (unsigned int)exponent;

    if (exponent < 0) {
        x = 1.0 / x;

        if (!isfinite(x)) {
            return STATUS_OVERFLOW;
        }
    }

    return power_recursive(x, magnitude, result);
}

Status bisection(double left, double right, double eps,
                 Equation function, double *result)
{
    if (function == NULL || result == NULL ||
        !isfinite(left) || !isfinite(right) ||
        !isfinite(eps) || eps <= 0.0) {
        return STATUS_INVALID_ARGUMENT;
    }

    if ((long double)right - left <= eps) {
        return STATUS_INVALID_ARGUMENT;
    }

    double f_left;
    double f_right;

    Status status = function(left, &f_left);
    if (status != STATUS_OK) {
        return status;
    }

    status = function(right, &f_right);
    if (status != STATUS_OK) {
        return status;
    }

    if (!isfinite(f_left) || !isfinite(f_right)) {
        return STATUS_OVERFLOW;
    }

    /* Точные нули на концах дают точные корни. */
    if (f_left == 0.0) {
        *result = left;
        return STATUS_OK;
    }

    if (f_right == 0.0) {
        *result = right;
        return STATUS_OK;
    }

    if (signbit(f_left) == signbit(f_right)) {
        return STATUS_NO_ROOT;
    }

    for (int iteration = 0; iteration < 2048; ++iteration) {
        double middle = left / 2.0 + right / 2.0;

        if (((long double)right - left) / 2.0L <= eps) {
            *result = middle;
            return STATUS_OK;
        }

        if (middle == left || middle == right) {
            return STATUS_PRECISION_ERROR;
        }

        double f_middle;
        status = function(middle, &f_middle);

        if (status != STATUS_OK) {
            return status;
        }

        if (!isfinite(f_middle)) {
            return STATUS_OVERFLOW;
        }

        if (f_middle == 0.0) {
            *result = middle;
            return STATUS_OK;
        }

        if (signbit(f_left) != signbit(f_middle)) {
            right = middle;
        } else {
            left = middle;
            f_left = f_middle;
        }
    }

    return STATUS_ITERATION_LIMIT;
}