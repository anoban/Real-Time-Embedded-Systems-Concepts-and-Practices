// gcc test.c -Wall -Wpedantic -O3 -std=c17 -o test.exe

#include <stdio.h>
#include <stdlib.h>

static inline long double factorial(unsigned short value) {
    if (value == 0) return 1.0L;
    long double result = 1.000L;
    for (size_t i = 1; i <= value; ++i) result *= i;
    return result;
}

int main(void) {
    for (int i = 0; i < 200; ++i)
        printf_s("Factorial of %4d is %10.6LE\n" /* LE is needed for gcc, E alone won't work since it is for floats */, i, factorial(i));
    return EXIT_SUCCESS;
}
