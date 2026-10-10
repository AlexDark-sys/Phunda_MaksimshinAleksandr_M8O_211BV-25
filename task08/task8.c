#include "task8.h"

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

Status read_token(FILE *file, char **result)
{
    if (file == NULL || result == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    *result = NULL;

    int ch;

    do {
        ch = fgetc(file);
    } while (ch == ' ' || ch == '\t' ||
             ch == '\n' || ch == '\r');

    if (ch == EOF) {
        return ferror(file)
            ? STATUS_IO_ERROR
            : STATUS_END_OF_FILE;
    }

    size_t capacity = 16;
    size_t length = 0;

    char *buffer = malloc(capacity);

    if (buffer == NULL) {
        return STATUS_MEMORY_ERROR;
    }

    while (ch != EOF && ch != ' ' && ch != '\t' &&
           ch != '\n' && ch != '\r') {
        if (ch == '\0') {
            free(buffer);
            return STATUS_INVALID_NUMBER;
        }

        if (length == capacity - 1) {
            if (capacity > SIZE_MAX / 2) {
                free(buffer);
                return STATUS_OVERFLOW;
            }

            size_t new_capacity = capacity * 2;
            char *temp = realloc(buffer, new_capacity);

            if (temp == NULL) {
                free(buffer);
                return STATUS_MEMORY_ERROR;
            }

            buffer = temp;
            capacity = new_capacity;
        }

        buffer[length++] = (char)ch;
        ch = fgetc(file);
    }

    if (ferror(file)) {
        free(buffer);
        return STATUS_IO_ERROR;
    }

    buffer[length] = '\0';
    *result = buffer;

    return STATUS_OK;
}

Status digit_value(char symbol, int *result)
{
    if (result == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (symbol >= '0' && symbol <= '9') {
        *result = symbol - '0';
    } else if (symbol >= 'A' && symbol <= 'Z') {
        *result = symbol - 'A' + 10;
    } else if (symbol >= 'a' && symbol <= 'z') {
        *result = symbol - 'a' + 10;
    } else {
        return STATUS_INVALID_NUMBER;
    }

    return STATUS_OK;
}

Status determine_base(const char *token, int *base)
{
    if (token == NULL || base == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    size_t i = 0;

    if (token[i] == '+' || token[i] == '-') {
        ++i;
    }

    if (token[i] == '\0') {
        return STATUS_INVALID_NUMBER;
    }

    int max_digit = 0;

    for (; token[i] != '\0'; ++i) {
        int digit;
        Status status = digit_value(token[i], &digit);

        if (status != STATUS_OK) {
            return status;
        }

        if (digit > max_digit) {
            max_digit = digit;
        }
    }

    *base = max_digit + 1;

    if (*base < 2) {
        *base = 2;
    }

    return STATUS_OK;
}

Status convert_number(const char *token, int base,
                      long long *result)
{
    if (token == NULL || result == NULL ||
        base < 2 || base > 36) {
        return STATUS_INVALID_ARGUMENT;
    }

    size_t i = 0;
    int negative = 0;

    if (token[i] == '+' || token[i] == '-') {
        negative = (token[i] == '-');
        ++i;
    }

    if (token[i] == '\0') {
        return STATUS_INVALID_NUMBER;
    }

    unsigned long long value = 0;

    unsigned long long limit = negative
        ? (unsigned long long)LLONG_MAX + 1ULL
        : (unsigned long long)LLONG_MAX;

    for (; token[i] != '\0'; ++i) {
        int digit;
        Status status = digit_value(token[i], &digit);

        if (status != STATUS_OK) {
            return status;
        }

        if (digit >= base) {
            return STATUS_INVALID_NUMBER;
        }

        if (value > (limit - (unsigned int)digit)
                    / (unsigned int)base) {
            return STATUS_OVERFLOW;
        }

        value = value * (unsigned int)base
                + (unsigned int)digit;
    }

    if (negative) {
        if (value == (unsigned long long)LLONG_MAX + 1ULL) {
            *result = LLONG_MIN;
        } else {
            *result = -(long long)value;
        }
    } else {
        *result = (long long)value;
    }

    return STATUS_OK;
}

Status normalize_number(char *token)
{
    if (token == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    int base;
    Status status = determine_base(token, &base);

    if (status != STATUS_OK) {
        return status;
    }

    int negative = (token[0] == '-');
    size_t start = (token[0] == '+' || token[0] == '-') ? 1 : 0;

    while (token[start] == '0' && token[start + 1] != '\0') {
        ++start;
    }

    int is_zero = (token[start] == '0' &&
                   token[start + 1] == '\0');

    size_t destination = (negative && !is_zero) ? 1 : 0;

    memmove(token + destination,
            token + start,
            strlen(token + start) + 1);

    if (destination == 1) {
        token[0] = '-';
    }

    return STATUS_OK;
}