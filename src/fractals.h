#ifndef FRACTALS_H
#define FRACTALS_H

#include <math.h>

int mandelbrot(long double real, long double imaginary, int max_iterations);
int julia(long double real, long double imaginary, int max_iterations, long double c_real, long double c_imaginary);
int burning_ship(long double real, long double imaginary, int max_iterations);

#endif /* FRACTALS_H */
