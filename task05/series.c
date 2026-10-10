#include "task5.h"

#include <stddef.h>
#include <float.h>
#include <math.h>

static Status get_ratio(int kind, unsigned long n,
                        long double x, long double *result)
{
    if (result == NULL || !isfinite(x) ||
        kind < 0 || kind > 3) {
        return INVALID_ARGUMENT;
    }

    long double k = (long double)n;
    long double ratio;

    switch (kind) {
        case 0:
            ratio = x / (k + 1.0L);
            break;

        case 1:
            ratio = -(x / (2.0L * k + 1.0L))
                     * (x / (2.0L * k + 2.0L));
            break;

        case 2:
            ratio = (3.0L * (k + 1.0L) * x
                     / (3.0L * k + 1.0L))
                    * (3.0L * (k + 1.0L) * x
                       / (3.0L * k + 2.0L));
            break;

        default:
            ratio = -x * x
                    * (2.0L * k + 1.0L)
                    / (2.0L * k + 2.0L);
            break;
    }

    if (!isfinite(ratio)) {
        return PRECISION_ERROR;
    }

    *result = ratio;
    return OK;
}

Status sum_series(int kind, long double x,
                  long double eps, long double *result)
{
    if (result == NULL || !isfinite(x) ||
        !isfinite(eps) || eps <= 0.0L ||
        kind < 0 || kind > 3) {
        return INVALID_ARGUMENT;
    }

    if ((kind == 2 && fabsl(x) >= 1.0L) ||
        (kind == 3 && fabsl(x) > 1.0L)) {
        return DIVERGES;
    }

    if (eps < 64.0L * LDBL_EPSILON) {
        return PRECISION_ERROR;
    }

    unsigned long n = kind == 3 ? 1UL : 0UL;
    long double term = kind == 3 ? -x * x / 2.0L : 1.0L;
    long double sum = term;
    long double correction = 0.0L;
    long double absolute_sum = fabsl(term);

    const unsigned long max_iterations = 10000000UL;

    for (unsigned long i = 0; i < max_iterations; ++i) {
        long double ratio;
        Status status = get_ratio(kind, n, x, &ratio);

        if (status != OK) {
            return status;
        }

        long double next = term * ratio;

        if (!isfinite(next)) {
            return PRECISION_ERROR;
        }

        long double tail_bound = INFINITY;

        if (kind == 1 || kind == 3) {
            /* Оценка остатка знакочередующегося ряда. */
            if (fabsl(ratio) <= 1.0L) {
                tail_bound = fabsl(next);
            }
        } else {
            /* Оценка хвоста через геометрическую прогрессию. */
            long double following_ratio;

            status = get_ratio(kind, n + 1UL, x,
                               &following_ratio);

            if (status != OK) {
                return status;
            }

            long double q = fabsl(following_ratio);

            if (q < 1.0L) {
                tail_bound = fabsl(next) / (1.0L - q);
            }
        }

        /* Практическая оценка влияния округления. */
        long double rounding_estimate =
            64.0L * LDBL_EPSILON * absolute_sum;

        if (rounding_estimate > eps / 2.0L) {
            return PRECISION_ERROR;
        }

        if (tail_bound <= eps / 2.0L) {
            *result = sum;
            return OK;
        }

        /* Суммирование Кэхэна. */
        long double adjusted = next - correction;
        long double updated = sum + adjusted;

        correction = (updated - sum) - adjusted;
        sum = updated;
        absolute_sum += fabsl(next);

        if (!isfinite(sum) || !isfinite(absolute_sum)) {
            return PRECISION_ERROR;
        }

        term = next;
        ++n;
    }

    return ITERATION_LIMIT;
}