#include "task8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s input.txt output.txt\n",
                argv[0]);
        return 1;
    }

    if (strcmp(argv[1], argv[2]) == 0) {
        fprintf(stderr, "Input and output paths must differ.\n");
        return 1;
    }

    FILE *input = fopen(argv[1], "r");

    if (input == NULL) {
        fprintf(stderr, "Cannot open input file.\n");
        return 1;
    }

    FILE *temporary = tmpfile();

    if (temporary == NULL) {
        fprintf(stderr, "Cannot create temporary file.\n");

        if (fclose(input) == EOF) {
            fprintf(stderr, "Cannot close input file.\n");
        }

        return 1;
    }

    Status status = STATUS_OK;

    while (status == STATUS_OK) {
        char *token = NULL;

        status = read_token(input, &token);

        if (status == STATUS_END_OF_FILE) {
            status = STATUS_OK;
            break;
        }

        if (status != STATUS_OK) {
            free(token);
            break;
        }

        int base;
        long long decimal;

        status = determine_base(token, &base);

        if (status == STATUS_OK) {
            status = convert_number(token, base, &decimal);
        }

        if (status == STATUS_OK) {
            status = normalize_number(token);
        }

        if (status == STATUS_OK) {
            if (fprintf(temporary, "%s %d %lld\n",
                        token, base, decimal) < 0) {
                status = STATUS_IO_ERROR;
            }
        } else {
            fprintf(stderr, "Invalid number: %s. Status: %d\n",
                    token, (int)status);
        }

        free(token);
    }

    if (fclose(input) == EOF && status == STATUS_OK) {
        status = STATUS_IO_ERROR;
    }

    if (status == STATUS_OK) {
        if (fseek(temporary, 0, SEEK_SET) != 0) {
            status = STATUS_IO_ERROR;
        }
    }

    FILE *output = NULL;

    if (status == STATUS_OK) {
        output = fopen(argv[2], "w");

        if (output == NULL) {
            fprintf(stderr, "Cannot open output file.\n");
            status = STATUS_IO_ERROR;
        }
    }

    if (status == STATUS_OK) {
        int ch;

        while ((ch = fgetc(temporary)) != EOF) {
            if (fputc(ch, output) == EOF) {
                status = STATUS_IO_ERROR;
                break;
            }
        }

        if (ferror(temporary)) {
            status = STATUS_IO_ERROR;
        }
    }

    if (output != NULL) {
        if (fclose(output) == EOF && status == STATUS_OK) {
            status = STATUS_IO_ERROR;
        }
    }

    if (fclose(temporary) == EOF && status == STATUS_OK) {
        status = STATUS_IO_ERROR;
    }

    if (status != STATUS_OK) {
        fprintf(stderr, "Processing failed. Status: %d\n",
                (int)status);
        return 1;
    }

    return 0;
}