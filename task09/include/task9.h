#ifndef TASK9_H
#define TASK9_H

#include <stddef.h>

typedef enum {
    STATUS_OK,
    STATUS_INVALID_ARGUMENT,
    STATUS_INVALID_NUMBER,
    STATUS_OVERFLOW,
    STATUS_MEMORY_ERROR,
    STATUS_IO_ERROR
} Status;

Status parse_double(const char *str, double *value);

Status fill_double_array(double *array, size_t size,
                         double a, double b, double eps);

Status swap_min_max(double *array, size_t size, double eps,
                    size_t *max_index, size_t *min_index);

Status print_double_array(const double *array, size_t size);

Status fill_int_array(int *array, size_t size);

Status create_array_c(const int *a, size_t size_a,
                      const int *b, size_t size_b,
                      int **result);

Status print_int_array(const int *array, size_t size);

#endif