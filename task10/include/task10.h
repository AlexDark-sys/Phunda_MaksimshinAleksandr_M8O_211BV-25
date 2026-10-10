#ifndef TASK10_H
#define TASK10_H

#include <stddef.h>
#include <stdio.h>

typedef enum {
    STATUS_OK,
    STATUS_END_OF_FILE,
    STATUS_INVALID_ARGUMENT,
    STATUS_INVALID_NUMBER,
    STATUS_OVERFLOW,
    STATUS_MEMORY_ERROR,
    STATUS_IO_ERROR
} Status;

Status read_token(FILE *stream, char **result);

Status parse_base(const char *text, int *result);

Status parse_number(const char *text, int base,
                    long long *result);

Status number_to_string(long long number, int base,
                        char *buffer, size_t capacity);

Status add_numbers(long long a, long long b,
                   long long *result);

Status get_magnitude(long long number,
                     unsigned long long *result);

Status print_result(const char *name, long long number,
                    int original_base);

#endif