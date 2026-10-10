#include "task5.h"

#include <stddef.h>
#include <float.h>
#include <math.h>

static int valid_point(long double x, const long double *result)
{
    return result != NULL && isfinite(x) &&
           x >= 0.0L && x <= 1.0L;
}

Status function_a(long double x, long double *result)
{
    if (!valid_point(x, result)) {
        return INVALID_ARGUMENT;
    }

    *result = x == 0.0L ? 1.0L : log1pl(x) / x;
    return OK;
}

Status function_b(long double x, long double *result)
{
    if (!valid_point(x, result)) {
        return INVALID_ARGUMENT;
    }

    *result = expl(-x * x / 2.0L);
    return OK;
}

Status function_c(long double t, long double *result)
{
    if (!valid_point(t, result)) {
        return INVALID_ARGUMENT;
    }

    /* Замена x = 1 - t^4. */
    *result = t == 0.0L
        ? 0.0L
        : -16.0L * t * t * t * logl(t);

    return OK;
}

Status function_d(long double x, long double *result)
{
    if (!valid_point(x, result)) {
        return INVALID_ARGUMENT;
    }

    *result = x == 0.0L ? 1.0L : expl(x * logl(x));
    return OK;
}

static Status adaptive_simpson(
    Function function,
    long double left,
    long double right,
    long double f_left,
    long double f_middle,
    long double f_right,
    long double whole,
    long double eps,
    unsigned int depth,
    unsigned long *budget,
    long double *result)
{
    if (function == NULL || budget == NULL || result == NULL ||
        !isfinite(left) || !isfinite(right) || left >= right ||
        !isfinite(f_left) || !isfinite(f_middle) ||
        !isfinite(f_right) || !isfinite(whole) ||
        !isfinite(eps) || eps <= 0.0L) {
        return INVALID_ARGUMENT;
    }

    if (*budget < 2UL) {
        return ITERATION_LIMIT;
    }

    *budget -= 2UL;

    long double middle = left + (right - left) / 2.0L;
    long double lm = left + (middle - left) / 2.0L;
    long double rm = middle + (right - middle) / 2.0L;

    if (lm == left || lm == middle ||
        rm == middle || rm == right) {
        return PRECISION_ERROR;
    }

    long double flm;
    long double frm;

    Status status = function(lm, &flm);

    if (status != OK) {
        return status;
    }

    status = function(rm, &frm);

    if (status != OK) {
        return status;
    }

    if (!isfinite(flm) || !isfinite(frm)) {
        return PRECISION_ERROR;
    }

    long double left_sum =
        (middle - left) / 6.0L *
        (f_left + 4.0L * flm + f_middle);

    long double right_sum =
        (right - middle) / 6.0L *
        (f_middle + 4.0L * frm + f_right);

    long double refined = left_sum + right_sum;
    long double difference = refined - whole;

    if (!isfinite(refined) || !isfinite(difference)) {
        return PRECISION_ERROR;
    }

    if (fabsl(difference) / 15.0L <= eps) {
        *result = refined + difference / 15.0L;
        return isfinite(*result) ? OK : PRECISION_ERROR;
    }

    if (depth == 0) {
        return ITERATION_LIMIT;
    }

    long double left_result;
    long double right_result;

    status = adaptive_simpson(
        function, left, middle,
        f_left, flm, f_middle,
        left_sum, eps / 2.0L, depth - 1,
        budget, &left_result
    );

    if (status != OK) {
        return status;
    }

    status = adaptive_simpson(
        function, middle, right,
        f_middle, frm, f_right,
        right_sum, eps / 2.0L, depth - 1,
        budget, &right_result
    );

    if (status != OK) {
        return status;
    }

    *result = left_result + right_result;
    return isfinite(*result) ? OK : PRECISION_ERROR;
}

Status integrate(Function function, long double eps,
                 long double *result)
{
    if (function == NULL || result == NULL ||
        !isfinite(eps) || eps <= 0.0L) {
        return INVALID_ARGUMENT;
    }

    if (eps < 64.0L * LDBL_EPSILON) {
        return PRECISION_ERROR;
    }

    long double f_left;
    long double f_middle;
    long double f_right;

    Status status = function(0.0L, &f_left);

    if (status != OK) {
        return status;
    }

    status = function(0.5L, &f_middle);

    if (status != OK) {
        return status;
    }

    status = function(1.0L, &f_right);

    if (status != OK) {
        return status;
    }

    if (!isfinite(f_left) || !isfinite(f_middle) ||
        !isfinite(f_right)) {
        return PRECISION_ERROR;
    }

    long double whole =
        (f_left + 4.0L * f_middle + f_right) / 6.0L;

    unsigned long budget = 1000000UL;

    return adaptive_simpson(
        function, 0.0L, 1.0L,
        f_left, f_middle, f_right,
        whole, eps, 60, &budget, result
    );
}