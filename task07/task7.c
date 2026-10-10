#include "task7.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

Status parse_flag(const char *str, char *flag)
{
    if (str == NULL || flag == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (strlen(str) != 2 ||
        (str[0] != '-' && str[0] != '/') ||
        (str[1] != 'r' && str[1] != 'a')) {
        return STATUS_INVALID_FLAG;
    }

    *flag = str[1];
    return STATUS_OK;
}

Status read_token(FILE *input, char **result)
{
    if (input == NULL || result == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    *result = NULL;

    int ch;

    do {
        ch = fgetc(input);
    } while (ch == ' ' || ch == '\t' ||
             ch == '\n' || ch == '\r');

    if (ch == EOF) {
        return ferror(input)
            ? STATUS_FILE_READ_ERROR
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
            return STATUS_INVALID_ARGUMENT;
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
        ch = fgetc(input);
    }

    if (ferror(input)) {
        free(buffer);
        return STATUS_FILE_READ_ERROR;
    }

    buffer[length] = '\0';
    *result = buffer;

    return STATUS_OK;
}

/*
 * position — номер лексемы внутри повторяющегося
 * блока из 10 лексем: от 1 до 10.
 *
 * Эта функция только преобразует данные, без ввода/вывода.
 */
Status transform_token(const char *token, unsigned int position,
                       char **result)
{
    if (token == NULL || result == NULL ||
        position < 1 || position > 10) {
        return STATUS_INVALID_ARGUMENT;
    }

    *result = NULL;

    size_t length = strlen(token);

    if (length == 0) {
        return STATUS_INVALID_ARGUMENT;
    }

    int lower = (position % 2 == 0);
    unsigned int base = 0;

    if (position == 10) {
        base = 4;
    } else if (position == 5) {
        base = 8;
    }

    /*
     * CHAR_BIT символов на байт достаточно для записи
     * даже в двоичной системе; для оснований 4 и 8 — тем более.
     */
    size_t factor = base == 0 ? 1 : CHAR_BIT;

    if (length > (SIZE_MAX - 1) / factor) {
        return STATUS_OVERFLOW;
    }

    char *buffer = malloc(length * factor + 1);

    if (buffer == NULL) {
        return STATUS_MEMORY_ERROR;
    }

    size_t written = 0;

    for (size_t i = 0; i < length; ++i) {
        unsigned char ch = (unsigned char)token[i];

        if (lower && ch >= 'A' && ch <= 'Z') {
            ch = (unsigned char)(ch - 'A' + 'a');
        }

        if (base == 0) {
            buffer[written++] = (char)ch;
        } else {
            char reversed[CHAR_BIT];
            size_t digits = 0;
            unsigned int code = ch;

            do {
                reversed[digits++] =
                    (char)('0' + code % base);
                code /= base;
            } while (code != 0);

            while (digits > 0) {
                buffer[written++] = reversed[--digits];
            }
        }
    }

    buffer[written] = '\0';
    *result = buffer;

    return STATUS_OK;
}

static Status write_token(FILE *output, const char *token,
                          int *first_token)
{
    if (output == NULL || token == NULL || first_token == NULL) {
        return STATUS_INVALID_ARGUMENT;
    }

    if (!*first_token) {
        if (fputc(' ', output) == EOF) {
            return STATUS_FILE_WRITE_ERROR;
        }
    }

    if (fputs(token, output) == EOF) {
        return STATUS_FILE_WRITE_ERROR;
    }

    *first_token = 0;
    return STATUS_OK;
}

Status merge_files(FILE *first, FILE *second, FILE *output)
{
    if (first == NULL || second == NULL || output == NULL ||
        first == second || first == output || second == output) {
        return STATUS_INVALID_ARGUMENT;
    }

    FILE *inputs[2] = {first, second};
    int finished[2] = {0, 0};
    int first_token = 1;

    while (!finished[0] || !finished[1]) {
        for (size_t i = 0; i < 2; ++i) {
            if (finished[i]) {
                continue;
            }

            char *token = NULL;
            Status status = read_token(inputs[i], &token);

            if (status == STATUS_END_OF_FILE) {
                finished[i] = 1;
                continue;
            }

            if (status != STATUS_OK) {
                free(token);
                return status;
            }

            status = write_token(output, token, &first_token);
            free(token);

            if (status != STATUS_OK) {
                return status;
            }
        }
    }

    return STATUS_OK;
}

Status transform_file(FILE *input, FILE *output)
{
    if (input == NULL || output == NULL || input == output) {
        return STATUS_INVALID_ARGUMENT;
    }

    unsigned int position = 1;
    int first_token = 1;

    while (1) {
        char *token = NULL;
        Status status = read_token(input, &token);

        if (status == STATUS_END_OF_FILE) {
            return STATUS_OK;
        }

        if (status != STATUS_OK) {
            free(token);
            return status;
        }

        char *transformed = NULL;

        status = transform_token(token, position, &transformed);
        free(token);

        if (status == STATUS_OK) {
            status = write_token(output, transformed, &first_token);
        }

        free(transformed);

        if (status != STATUS_OK) {
            return status;
        }

        /* Правила повторяются через каждые 10 лексем. */
        position = position == 10 ? 1 : position + 1;
    }
}