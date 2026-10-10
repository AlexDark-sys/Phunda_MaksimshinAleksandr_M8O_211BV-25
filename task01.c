/// דאָס לאַבאָראַטאָריע איז טייל געשריבן געוואָרן דורך מיר, טייל דורך געמיני. נו, וואָס צו טאָן, דאָס לעבן איז שווער.

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <stdbool.h>
#include <math.h>

long valid_x(char* str_x, long* long_x) {
    char* endptr;
    *long_x = strtol(str_x, &endptr, 10);

    if (endptr == str_x) { return 1; }
    else if (*endptr != '\0') { return 1; }
    else if ((*__errno_location()) == ERANGE && (*long_x == LONG_MAX || *long_x == LONG_MIN)) { return 1; }
    return 0;
}

char valid_flag(char* str_flag, char* str_flag_valid) {
    if (str_flag[0] != '-' && str_flag[0] != '/') { return 1; }
    else if (str_flag[2] != 0) { return 1; }
    else { *str_flag_valid = str_flag[1]; return 0; }
}

int multiples_x(long x, int mult[]) {
    if (x <= 0) return 0; // Защита от деления на 0 и отрицательных чисел
    int i = 100 / (int)x;
    if (i > 0) {
        for (; i > 0; i--) {
            mult[i - 1] = x * i;
        }
        return 100 / (int)x;
    }
    else return 0;
}

void print_arr(int arr[], int cnt) {
    int i = 0;
    for (; i < cnt; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int is_prime(long x) {
    if (x <= 1) return -1; // Числа <= 1 
    long i = 2, x_sqrt = (long)sqrt(x);
    while (i <= x_sqrt) {
        if (x % i == 0) return 0;
        i++; // 
    }
    return 1;
}

// Флаг -s: Получение шестнадцатеричных цифр числа
int get_hex_digits(long x, char digits[]) {
    bool sign = 0;
    if (x < 0) {sign = 1; x *= -1;}; // Будем обрабатывать только неотрицательные числа
    if (x == 0) {
        digits[0] = '0';
        return 1;
    }
    int cnt = 0;
    long temp = x;
    while (temp > 0) {
        int rem = temp % 16;
        if (rem < 10) {
            digits[cnt] = '0' + rem;
        } else {
            digits[cnt] = 'A' + (rem - 10);
        }
        cnt++;
        temp /= 16;
    }
    if (sign == 1) {digits[cnt] = '-';cnt++;}
    // Разворачиваем массив
    int i = 0;
    for (; i < cnt / 2; i++) {
        char t = digits[i];
        digits[i] = digits[cnt - 1 - i];
        digits[cnt - 1 - i] = t;
    }
    return cnt;
}

void print_hex_digits(char digits[], int cnt) {
    int i = 0;
    for (; i < cnt; i++) {
        printf("%c ", digits[i]);
    }
    printf("\n");
}

// Флаг -e: Заполнение таблицы степеней
int power_table(long x, long long table[10][10]) {
    if (x < 1 || x > 10) return 1; // Ограничение по условию задачи
    long base = 1;
    for (; base <= 10; base++) {
        long long val = 1;
        long exp = 1;
        for (; exp <= x; exp++) {
            val *= base;
            table[base - 1][exp - 1] = val;
        }
    }
    return 0;
}

void print_power_table(long long table[10][10], long x) {
    long base = 1;
    for (; base <= 10; base++) {
        printf("Основание %2ld:", base);
        long exp = 1;
        for (; exp <= x; exp++) {
            printf(" %-11lld", table[base - 1][exp - 1]);
        }
        printf("\n");
    }
}

// Флаг -a: Вычисление суммы натуральных чисел от 1 до x
int sum_natural(long x, long long* res) {
    if (x < 1) return 1;
    // Используем long long для предотвращения переполнения
    *res = ((long long)x * (x + 1)) / 2;
    return 0;
}

// Флаг -f: Вычисление факториала x
int factorial_x(long x, long long* res) {
    if (x < 0) return 1;
    if (x > 20) return 2; // Предел для 64-битного целого (long long) во избежание переполнения
    long long f = 1;
    long i = 1;
    for (; i <= x; i++) {
        f *= i;
    }
    *res = f;
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("there should be two arguments\n");
        return -1;
    }
    char* str_x = argv[1];
    long long_x = 0;
    if (valid_x(str_x, &long_x) != 0) { printf("invalid value for the first argument\n"); return -1; }

    char* str_flag = argv[2];
    char str_flag_valid;
    if (valid_flag(str_flag, &str_flag_valid) != 0) { printf("invalid value for the second argument\n"); return -1; }

    switch (str_flag_valid)
    {
    case 'h': {
        int mult[100];
        int cnt_mult;
        if ((cnt_mult = multiples_x(long_x, mult)) != 0) {
            print_arr(mult, cnt_mult);
        }
        else printf("there are no numbers that are multiples of %ld\n", long_x);
        break;
    }
    
    case 'p': {
        int prime_res = is_prime(long_x);
        if (prime_res == 1) { printf("%ld is a prime number\n", long_x); }
        else if (prime_res == 0) { printf("%ld is a composite number\n", long_x); }
        else { printf("%ld invalid argument\n", long_x); }
        break;
    }

    case 's': {
        char str_0xx[17]; // Достаточно для хранения цифр 64-битного числа
        int cnt_hex = get_hex_digits(long_x, str_0xx);
        if (cnt_hex > 0) {
            print_hex_digits(str_0xx, cnt_hex);
        } 
        else {
            printf("error");
        }
        break;
    }

    case 'e': {
        long long table[10][10];
        if (power_table(long_x, table) == 0) {
            print_power_table(table, long_x);
        } else {
            printf("error: x must be between 1 and 10 for this flag\n");
        }
        break;
    }

    case 'a': {
        long long sum_res;
        if (sum_natural(long_x, &sum_res) == 0) {
            printf("sum is: %lld\n", sum_res);
        } else {
            printf("error: x must be a natural number (>= 1)\n");
        }
        break;
    }

    case 'f': {
        long long fact_res;
        int status = factorial_x(long_x, &fact_res);
        if (status == 0) {
            printf("factorial is: %lld\n", fact_res);
        } else if (status == 2) {
            printf("error: overflow (x is too large for factorial computation)\n");
        } else {
            printf("error: x must be non-negative\n");
        }
        break;
    }

    default:
        printf("invalid flag\n");
        break;
    }

    return 0;
}