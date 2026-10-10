#include "task10.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    (void)argv;

    if (argc != 1) {
        fprintf(stderr, "This program takes no command-line arguments.\n");
        return 1;
    }

    if (printf("Enter base (2..36):\n") < 0 ||
        fflush(stdout) == EOF) {
        return 1;
    }

    char *token = NULL;
    Status status = read_token(stdin, &token);

    if (status != STATUS_OK) {
        fprintf(stderr, "Cannot read base.\n");
        free(token);
        return 1;
    }

    int base;
    status = parse_base(token, &base);
    free(token);
    token = NULL;

    if (status != STATUS_OK) {
        fprintf(stderr, "Invalid base: expected 2..36.\n");
        return 1;
    }

    if (printf("Enter numbers. Type Stop to finish:\n") < 0 ||
        fflush(stdout) == EOF) {
        return 1;
    }

    long long sum = 0;
    long long maximum = 0;
    unsigned long long max_magnitude = 0;
    int has_numbers = 0;

    while (1) {
        status = read_token(stdin, &token);

        if (status != STATUS_OK) {
            free(token);
            fprintf(stderr, "Input error: expected numbers or Stop.\n");
            return 1;
        }

        if (strcmp(token, "Stop") == 0) {
            free(token);
            token = NULL;
            break;
        }

        long long number;
        status = parse_number(token, base, &number);

        if (status != STATUS_OK) {
            fprintf(stderr, "Invalid number: %s. Status: %d\n",
                    token, (int)status);
            free(token);
            return 1;
        }

        free(token);
        token = NULL;

        long long new_sum;
        status = add_numbers(sum, number, &new_sum);

        if (status != STATUS_OK) {
            fprintf(stderr, "Sum overflow.\n");
            return 1;
        }

        unsigned long long magnitude;
        status = get_magnitude(number, &magnitude);

        if (status != STATUS_OK) {
            return 1;
        }

        if (!has_numbers || magnitude > max_magnitude) {
            maximum = number;
            max_magnitude = magnitude;
        }

        sum = new_sum;
        has_numbers = 1;
    }

    if (!has_numbers) {
        fprintf(stderr, "No numbers entered.\n");
        return 1;
    }

    status = print_result("Maximum by absolute value", maximum, base);

    if (status == STATUS_OK) {
        status = print_result("Sum", sum, base);
    }

    if (status != STATUS_OK || fflush(stdout) == EOF) {
        fprintf(stderr, "Output error.\n");
        return 1;
    }

    return 0;
}