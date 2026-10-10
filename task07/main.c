#include "task7.h"

#include <stdio.h>
#include <string.h>

static void print_error(Status status)
{
    switch (status) {
        case STATUS_INVALID_ARGUMENT:
            fprintf(stderr, "Invalid argument\n");
            break;

        case STATUS_INVALID_FLAG:
            fprintf(stderr, "Invalid flag: expected -r or -a\n");
            break;

        case STATUS_INVALID_ARGUMENT_COUNT:
            fprintf(stderr, "Invalid number of arguments\n");
            break;

        case STATUS_OVERFLOW:
            fprintf(stderr, "Overflow\n");
            break;

        case STATUS_MEMORY_ERROR:
            fprintf(stderr, "Memory allocation error\n");
            break;

        case STATUS_FILE_OPEN_ERROR:
            fprintf(stderr, "Cannot open file\n");
            break;

        case STATUS_FILE_READ_ERROR:
            fprintf(stderr, "File read error\n");
            break;

        case STATUS_FILE_WRITE_ERROR:
            fprintf(stderr, "File write error\n");
            break;

        case STATUS_FILE_CLOSE_ERROR:
            fprintf(stderr, "File close error\n");
            break;

        case STATUS_OK:
        case STATUS_END_OF_FILE:
            break;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        print_error(STATUS_INVALID_ARGUMENT_COUNT);
        return 1;
    }

    char flag;
    Status status = parse_flag(argv[1], &flag);

    if (status != STATUS_OK) {
        print_error(status);
        return 1;
    }

    if ((flag == 'r' && argc != 5) ||
        (flag == 'a' && argc != 4)) {
        print_error(STATUS_INVALID_ARGUMENT_COUNT);
        return 1;
    }

    const char *output_path = argv[argc - 1];

    if (argv[2][0] == '\0' || output_path[0] == '\0' ||
        (flag == 'r' && argv[3][0] == '\0')) {
        print_error(STATUS_INVALID_ARGUMENT);
        return 1;
    }

    if (strcmp(argv[2], output_path) == 0 ||
        (flag == 'r' && strcmp(argv[3], output_path) == 0)) {
        print_error(STATUS_INVALID_ARGUMENT);
        return 1;
    }

    FILE *first = fopen(argv[2], "r");

    if (first == NULL) {
        print_error(STATUS_FILE_OPEN_ERROR);
        return 1;
    }

    FILE *second = NULL;
    FILE *temporary = NULL;
    FILE *output = NULL;

    if (flag == 'r') {
        second = fopen(argv[3], "r");

        if (second == NULL) {
            status = STATUS_FILE_OPEN_ERROR;
        }
    }

    if (status == STATUS_OK) {
        temporary = tmpfile();

        if (temporary == NULL) {
            status = STATUS_FILE_OPEN_ERROR;
        }
    }

    if (status == STATUS_OK) {
        if (flag == 'r') {
            status = merge_files(first, second, temporary);
        } else {
            status = transform_file(first, temporary);
        }
    }

    int close_status = fclose(first);

    if (close_status != 0 && status == STATUS_OK) {
        status = STATUS_FILE_CLOSE_ERROR;
    }

    if (second != NULL) {
        close_status = fclose(second);

        if (close_status != 0 && status == STATUS_OK) {
            status = STATUS_FILE_CLOSE_ERROR;
        }
    }

    if (status == STATUS_OK) {
        if (fseek(temporary, 0, SEEK_SET) != 0) {
            status = STATUS_FILE_READ_ERROR;
        }
    }

    if (status == STATUS_OK) {
        output = fopen(output_path, "w");

        if (output == NULL) {
            status = STATUS_FILE_OPEN_ERROR;
        }
    }

    if (status == STATUS_OK) {
        int ch;

        while ((ch = fgetc(temporary)) != EOF) {
            if (fputc(ch, output) == EOF) {
                status = STATUS_FILE_WRITE_ERROR;
                break;
            }
        }

        if (ferror(temporary) && status == STATUS_OK) {
            status = STATUS_FILE_READ_ERROR;
        }
    }

    if (output != NULL) {
        close_status = fclose(output);

        if (close_status != 0 && status == STATUS_OK) {
            status = STATUS_FILE_CLOSE_ERROR;
        }
    }

    if (temporary != NULL) {
        close_status = fclose(temporary);

        if (close_status != 0 && status == STATUS_OK) {
            status = STATUS_FILE_CLOSE_ERROR;
        }
    }

    if (status != STATUS_OK) {
        print_error(status);
        return 1;
    }

    return 0;
}