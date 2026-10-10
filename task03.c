#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <limits.h>

#define OK 0
#define ERR_ARGS 1
#define ERR_FLAG 2
#define ERR_PARSE 3
#define ERR_DATA 4

int parse_double(const char *const str, double *const result) {
    char *endptr;
    *result = strtod(str, &endptr);
    if (*endptr != '\0' || endptr == str || 
        (*__errno_location()) == ERANGE && (*result == LONG_MAX || *result == LONG_MIN)) return ERR_PARSE;
    return OK;
}

// Функция безопасного парсинга целого числа
int parse_int(const char *const str, int *const result) {
    char *endptr;
    *result = (int)strtol(str, &endptr, 10);
    if (*endptr != '\0' || endptr == str || 
        (*__errno_location()) == ERANGE && (*result == LONG_MAX || *result == LONG_MIN)) return ERR_PARSE;
    return OK;
}

// Сравнение вещественных чисел
int double_equal(const double a, const double b, const double epsilon) {
    return fabs(a - b) < epsilon;
}

// Решение квадратного уравнения. Возвращает код ошибки, корни и их количество пишет в указатели.
// Количество корней: -1 — бесконечно много, 0 — нет, 1 или 2 — корни.
int solve_quadratic(const double a, const double b, const double c, const double epsilon, 
                    double *const x1, double *const x2, int *const root_count) {
    if (epsilon <= 0.0) return ERR_DATA;

    if (double_equal(a, 0.0, epsilon)) {
        if (double_equal(b, 0.0, epsilon)) {
            *root_count = double_equal(c, 0.0, epsilon) ? -1 : 0;
            return OK;
        }
        *x1 = -c / b;
        *root_count = 1;
        return OK;
    }

    double discr = b * b - 4 * a * c;
    if (discr < -epsilon) {
        *root_count = 0;
    } else if (double_equal(discr, 0.0, epsilon)) {
        *x1 = -b / (2 * a);
        *root_count = 1;
    } else {
        *x1 = (-b - sqrt(discr)) / (2 * a);
        *x2 = (-b + sqrt(discr)) / (2 * a);
        *root_count = 2;
    }
    return OK;
}

int print_solve_q(double x1, double x2, int cnt, double a, double b, double c) {
    if (cnt == -1) printf("Для набора %8.4f %8.4f %8.4f бесконечно много корней\n",a,b,c);
            else if (cnt == 0) printf("Для набора %8.4f %8.4f %8.4f нет действительных корней\n",a,b,c);
            else if (cnt == 1) printf("Для набора %8.4f %8.4f %8.4f -> x = %.4f\n",a,b,c, x1);
            else printf("Для набора %8.4f %8.4f %8.4f -> x1 = %8.4f, x2 = %8.4f\n",a,b,c, x1, x2);

}

// Проверка кратности двух ненулевых чисел
int check_divisibility(const int a, const int b, int *const is_divisible) {
    if (a == 0 || b == 0) return ERR_DATA;
    *is_divisible = (a % b == 0);
    return OK;
}

// Проверка на прямоугольный треугольник
int check_right_triangle(const double a, const double b, const double c, const double epsilon, int *const is_right) {
    if (epsilon <= 0.0 || a <= epsilon || b <= epsilon || c <= epsilon) return ERR_DATA;

    if (a > b && a > c) *is_right = double_equal(b * b + c * c, a * a, epsilon);
    else if (b > a && b > c) *is_right = double_equal(a * a + c * c, b * b, epsilon);
    else *is_right = double_equal(a * a + b * b, c * c, epsilon);
    
    return OK;
}


int decision_process(const double coefs[3], const double epsilon) {
    double history[6][3];
    int history_count = 0;
    int p[6][3] = {{0,1,2}, {0,2,1}, {1,0,2}, {1,2,0}, {2,0,1}, {2,1,0}};

    for (int i = 0; i < 6; i++) {
        double a = coefs[p[i][0]]; 
        double b = coefs[p[i][1]]; 
        double c = coefs[p[i][2]];
        
        int duplicate = 0;
        for (int j = 0; j < history_count; j++) {
            if (double_equal(history[j][0], a, epsilon) && 
                double_equal(history[j][1], b, epsilon) && 
                double_equal(history[j][2], c, epsilon)) {
                duplicate = 1; 
                break;
            }
        }
        if (duplicate) continue;

        history[history_count][0] = a; 
        history[history_count][1] = b; 
        history[history_count][2] = c;
        history_count++;

        double x1 = 0, x2 = 0;
        int cnt = 0;
        
        // Бизнес-логика: считаем корни
        int res = solve_quadratic(a, b, c, epsilon, &x1, &x2, &cnt);
        
        // Разделение контекста: передаем результаты расчёта в поток вывода
        if (res == OK) {
            print_solve_q(x1, x2, cnt, a, b, c);
        } else {
            return ERR_DATA;
        }
    }
    return OK;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Incorrect number of arguments.\n");
        return ERR_ARGS;
    }

    char *flag = argv[1];
    if ((flag[0] != '-' && flag[0] != '/') || strlen(flag) != 2) {
        printf("Invalid flag format.\n");
        return ERR_FLAG;
    }

    int status = OK;

        if (flag[1] == 'q') {
        if (argc != 6) return printf("The -q flag requires 4 numbers.\n"), ERR_ARGS;
        double eps, coefs[3];
        if (parse_double(argv[2], &eps) || parse_double(argv[3], &coefs[0]) || 
            parse_double(argv[4], &coefs[1]) || parse_double(argv[5], &coefs[2])) {
            printf("Number parsing error.\n");
            return ERR_PARSE;
        }
        
        // Исправлено: проверяем равенство статус-кода значению OK (0)
        status = decision_process(coefs, eps);
    }
    else if (flag[1] == 'm') {
        if (argc != 4) return printf("The -m flag requires 2 numbers.\n"), ERR_ARGS;
        int n1, n2, is_divisible;
        if (parse_int(argv[2], &n1) || parse_int(argv[3], &n2)) return printf("Number parsing error.\n"), ERR_PARSE;
        
        status = check_divisibility(n1, n2, &is_divisible);
        if (status == OK) printf("Число %d %sкратно %d.\n", n1, is_divisible ? "" : "НЕ ", n2);
    } 
    else if (flag[1] == 't') {
        if (argc != 6) return printf("The -t flag requires 4 numbers.\n"), ERR_ARGS;
        double eps, a, b, c; int is_right;
        if (parse_double(argv[2], &eps) || parse_double(argv[3], &a) || parse_double(argv[4], &b) || parse_double(argv[5], &c)) {
                printf("Number parsing error.\n");
                return  ERR_PARSE;
            }
        
        status = check_right_triangle(a, b, c, eps, &is_right);
        if (status == OK) {
            if (is_right) printf("The specified lengths can be the sides of a right triangle.");
            if (!is_right) printf("The specified lengths cannot be sides of a right-angled triangle.");
        }
    } 
    else {
        printf("Unknown flag\n");
        return ERR_FLAG;
    }

    if (status == ERR_DATA) printf("Incorrect logical data\n");

    return status;
}
