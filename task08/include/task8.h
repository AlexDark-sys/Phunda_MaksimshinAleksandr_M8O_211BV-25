#ifndef TASK8_H
#define TASK8_H

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

Status read_token(FILE *file, char **result);

Status digit_value(char symbol, int *result);

Status determine_base(const char *token, int *base);

Status convert_number(const char *token, int base,
                      long long *result);

Status normalize_number(char *token);

#endif