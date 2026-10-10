#include "task9.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>

Status parse_double(const char *str, double *value)
{
    if (str == NULL || value == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (str[0] == '\0' || isspace((unsigned char)str[0])) {
        return STATUS_INVALID_NUMBER;
    }

    size_t start = (str[0] == '+' || str[0] == '-') ? 1 : 0;

    if (str[start] == '0' &&
        (str[start + 1] == 'x' || str[start + 1] == 'X')) {
        return STATUS_INVALID_NUMBER;
    }

    char *end;
    double result = strtod(str, &end);

    if (end == str || *end != '\0') {
        return STATUS_INVALID_NUMBER;
    }

    if (isfinite(result)) {
        return STATUS_OVERFLOW;
    }

    /* Обнаруживаем ненулевую мантиссу, округлившуюся до нуля. */
    if (result == 0.0) {
        for (size_t i = 0;
             str[i] != '\0' && str[i] != 'e' && str[i] != 'E';
             ++i) {
            if (str[i] >= '1' && str[i] <= '9') {
                return STATUS_OVERFLOW;
            }
        }
    }

    *value = result;
    return STATUS_OK;
}

Status fill_double_array(double *array, size_t size,
                         double a, double b, double eps)
{
    if (array == NULL || size == 0 ||
        !isfinite(a) || !isfinite(b) ||
        !isfinite(eps) || eps <= 0.0) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (a > b) {
        if (fabs(a - b) > eps) {
            return STATUS_INVALID_ARGUMENT;
        }

        double temp = a;
        a = b;
        b = temp;
    }

    for (size_t i = 0; i < size; ++i) {
        double fraction = (double)rand() / (double)RAND_MAX;

        double number = (1.0 - fraction) * a + fraction * b;

        if (!isfinite(number)) {
            return STATUS_OVERFLOW;
        }

        array[i] = number;
    }

    return STATUS_OK;
}

Status swap_min_max(double *array, size_t size, double eps,
                    size_t *max_index, size_t *min_index)
{
    if (array == NULL || size == 0 ||
        max_index == NULL || min_index == NULL ||
        max_index == min_index ||
        !isfinite(eps) || eps <= 0.0) {
        return STATUS_INVALID_ARGUMENT;
    }

    size_t i_max = 0;
    size_t i_min = 0;

    for (size_t i = 0; i < size; ++i) {
        if (!isfinite(array[i])) {
            return STATUS_INVALID_ARGUMENT;
        }

        if (array[i] > array[i_max] &&
            fabs(array[i] - array[i_max]) > eps) {
            i_max = i;
        }

        if (array[i] < array[i_min] &&
            fabs(array[i] - array[i_min]) > eps) {
            i_min = i;
        }
    }

    double temp = array[i_max];
    array[i_max] = array[i_min];
    array[i_min] = temp;

    /* Возвращаем индексы до обмена. */
    *max_index = i_max;
    *min_index = i_min;

    return STATUS_OK;
}

Status print_double_array(const double *array, size_t size)
{
    if (array == NULL || size == 0) {
        return STATUS_INVALID_ARGUMENT;
    }

    for (size_t i = 0; i < size; ++i) {
        if (printf("%.5g%c", array[i],
                   i + 1 == size ? '\n' : ' ') < 0) {
            return STATUS_IO_ERROR;
        }
    }

    return STATUS_OK;
}

Status fill_int_array(int *array, size_t size)
{
    if (array == NULL || size < 10 || size > 10000) {
        return STATUS_INVALID_ARGUMENT;
    }

    for (size_t i = 0; i < size; ++i) {
        array[i] = rand() % 2001 - 1000;
    }

    return STATUS_OK;
}

Status create_array_c(const int *a, size_t size_a,
                      const int *b, size_t size_b,
                      int **result)
{
    if (result == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    *result = NULL;

    if (a == NULL || b == NULL ||
        size_a < 10 || size_a > 10000 ||
        size_b < 10 || size_b > 10000) {
        return STATUS_INVALID_ARGUMENT;
    }

    for (size_t i = 0; i < size_a; ++i) {
        if (a[i] < -1000 || a[i] > 1000) {
            return STATUS_INVALID_ARGUMENT;
        }
    }

    for (size_t i = 0; i < size_b; ++i) {
        if (b[i] < -1000 || b[i] > 1000) {
            return STATUS_INVALID_ARGUMENT;
        }
    }

    if (size_a > SIZE_MAX / sizeof(int)) {
        return STATUS_OVERFLOW;
    }

    int *c = malloc(size_a * sizeof(*c));

    if (c == NULL) {
        return STATUS_MEMORY_ERROR;
    }

    for (size_t i = 0; i < size_a; ++i) {
        int closest = b[0];
        int min_distance = abs(a[i] - b[0]);

        for (size_t j = 1;
             j < size_b && min_distance != 0;
             ++j) {
            int distance = abs(a[i] - b[j]);

            if (distance < min_distance) {
                min_distance = distance;
                closest = b[j];
            }
        }

        c[i] = a[i] + closest;
    }

    *result = c;
    return STATUS_OK;
}

Status print_int_array(const int *array, size_t size)
{
    if (array == NULL || size == 0) {
        return STATUS_INVALID_ARGUMENT;
    }

    for (size_t i = 0; i < size; ++i) {
        if (printf("%d%c", array[i],
                   i + 1 == size ? '\n' : ' ') < 0) {
            return STATUS_IO_ERROR;
        }
    }

    return STATUS_OK;
}