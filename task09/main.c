#include "task9.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s a b\n", argv[0]);
        return 1;
    }

    double left;
    double right;
    const double eps = 1e-9;

    Status status = parse_double(argv[1], &left);

    if (status != STATUS_OK) {
        fprintf(stderr, "Invalid a. Status: %d\n", (int)status);
        return 1;
    }

    status = parse_double(argv[2], &right);

    if (status != STATUS_OK) {
        fprintf(stderr, "Invalid b. Status: %d\n", (int)status);
        return 1;
    }

    srand((unsigned int)time(NULL));

    /* Первый подпункт. */
    double array[10];
    size_t size = sizeof(array) / sizeof(array[0]);

    status = fill_double_array(array, size, left, right, eps);

    if (status != STATUS_OK) {
        fprintf(stderr, "Cannot fill array. Status: %d\n",
                (int)status);
        return 1;
    }

    size_t i;
    size_t j;

    status = swap_min_max(array, size, eps, &i, &j);

    if (status != STATUS_OK) {
        fprintf(stderr, "Cannot swap minimum and maximum.\n");
        return 1;
    }

    /* После обмена максимум находится на j, минимум на i. */
    double max_number = array[j];
    double min_number = array[i];

    if (printf(
            "Before swap:\n"
            "max_number = %.10g   i = %zu\n"
            "min_number = %.10g   j = %zu\n\n"
            "After swap:\n"
            "max_number = %.10g   j = %zu\n"
            "min_number = %.10g   i = %zu\n",
            max_number, i,
            min_number, j,
            max_number, j,
            min_number, i) < 0) {
        return 1;
    }

    /* Второй подпункт. */
    size_t size_a = 10U + (size_t)(rand() % 9991);
    size_t size_b = 10U + (size_t)(rand() % 9991);

    if (size_a > SIZE_MAX / sizeof(int) ||
        size_b > SIZE_MAX / sizeof(int)) {
        fprintf(stderr, "Array size overflow.\n");
        return 1;
    }

    int *a = malloc(size_a * sizeof(*a));
    int *b = malloc(size_b * sizeof(*b));
    int *c = NULL;

    if (a == NULL || b == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(a);
        free(b);
        return 1;
    }

    status = fill_int_array(a, size_a);

    if (status == STATUS_OK) {
        status = fill_int_array(b, size_b);
    }

    if (status == STATUS_OK) {
        status = create_array_c(a, size_a, b, size_b, &c);
    }

    if (status == STATUS_OK) {
        size_t selected_index = (size_t)rand() % size_a;

        if (printf(
                "\nSecond part:\n"
                "i = %zu\n"
                "A[i] = %d\n"
                "Nearest B = %d\n"
                "C[i] = %d\n",
                selected_index,
                a[selected_index],
                c[selected_index] - a[selected_index],
                c[selected_index]) < 0) {
            status = STATUS_IO_ERROR;
        }
    }

    free(a);
    free(b);
    free(c);

    if (status != STATUS_OK) {
        fprintf(stderr, "Second part failed. Status: %d\n",
                (int)status);
        return 1;
    }

    if (fflush(stdout) == EOF) {
        fprintf(stderr, "Output error.\n");
        return 1;
    }

    return 0;
}