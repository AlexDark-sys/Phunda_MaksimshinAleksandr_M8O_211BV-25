#include "task5.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

Status parse_number(const char *text, long double *result)
{
    if (text == NULL || result == NULL ||
        text[0] == '\0' || isspace((unsigned char)text[0])) {
        return INVALID_ARGUMENT;
    }

    size_t start = (text[0] == '+' || text[0] == '-') ? 1 : 0;

    if (text[start] == '0' &&
        (text[start + 1] == 'x' || text[start + 1] == 'X')) {
        return INVALID_ARGUMENT;
    }

    char *end;
    long double value = strtold(text, &end);

    if (end == text || *end != '\0' || !isfinite(value)) {
        return INVALID_ARGUMENT;
    }

    /* Ненулевое число могло округлиться до нуля. */
    if (value == 0.0L) {
        for (size_t i = 0;
             text[i] != '\0' && text[i] != 'e' && text[i] != 'E';
             ++i) {
            if (text[i] >= '1' && text[i] <= '9') {
                return INVALID_ARGUMENT;
            }
        }
    }

    *result = value;
    return OK;
}

static Status print_answer(int kind, Status status,
                           long double result)
{
    if (printf("%c: ", 'a' + kind) < 0) {
        return INVALID_ARGUMENT;
    }

    int written;

    switch (status) {
        case OK:
            written = printf("%.18Lg\n", result);
            break;

        case INVALID_ARGUMENT:
            written = printf("invalid arguments\n");
            break;

        case DIVERGES:
            written = printf("series diverges for this x\n");
            break;

        case PRECISION_ERROR:
            written = printf("requested precision cannot be ensured\n");
            break;

        case ITERATION_LIMIT:
            written = printf("iteration limit exceeded\n");
            break;

        default:
            return INVALID_ARGUMENT;
    }

    return written < 0 ? INVALID_ARGUMENT : OK;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,
                "Usage:\n"
                "  %s -s epsilon x\n"
                "  %s -i epsilon\n",
                argv[0], argv[0]);
        return 1;
    }

    int series_mode;

    if (strcmp(argv[1], "-s") == 0) {
        series_mode = 1;
    } else if (strcmp(argv[1], "-i") == 0) {
        series_mode = 0;
    } else {
        fprintf(stderr, "Expected -s or -i.\n");
        return 1;
    }

    if ((series_mode && argc != 4) ||
        (!series_mode && argc != 3)) {
        fprintf(stderr, "Invalid number of arguments.\n");
        return 1;
    }

    long double eps;

    if (parse_number(argv[2], &eps) != OK || eps <= 0.0L) {
        fprintf(stderr, "Epsilon must be finite and positive.\n");
        return 1;
    }

    long double x = 0.0L;

    if (series_mode && parse_number(argv[3], &x) != OK) {
        fprintf(stderr, "Invalid x.\n");
        return 1;
    }

    Function functions[] = {
        function_a, function_b, function_c, function_d
    };

    int failed = 0;

    for (int kind = 0; kind < 4; ++kind) {
        long double result = 0.0L;
        Status status;

        if (series_mode) {
            status = sum_series(kind, x, eps, &result);
        } else {
            status = integrate(functions[kind], eps, &result);
        }

        if (print_answer(kind, status, result) != OK) {
            fprintf(stderr, "Output error.\n");
            return 1;
        }

        if (status != OK) {
            failed = 1;
        }
    }

    if (fflush(stdout) == EOF) {
        fprintf(stderr, "Output error.\n");
        return 1;
    }

    return failed;
}