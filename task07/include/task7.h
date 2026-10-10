#ifndef TASK7_H
#define TASK7_H

#include <stdio.h>
#include <stddef.h>

typedef enum {
    STATUS_OK = 0,
    STATUS_END_OF_FILE,
    STATUS_INVALID_ARGUMENT,
    STATUS_INVALID_FLAG,
    STATUS_INVALID_ARGUMENT_COUNT,
    STATUS_OVERFLOW,
    STATUS_MEMORY_ERROR,
    STATUS_FILE_OPEN_ERROR,
    STATUS_FILE_READ_ERROR,
    STATUS_FILE_WRITE_ERROR,
    STATUS_FILE_CLOSE_ERROR
} Status;

Status parse_flag(const char *str, char *flag);

Status read_token(FILE *input, char **result);

Status transform_token(const char *token, unsigned int position,
                       char **result);

Status merge_files(FILE *first, FILE *second, FILE *output);

Status transform_file(FILE *input, FILE *output);

#endif