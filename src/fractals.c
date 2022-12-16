#include "fractals.h"

// https://en.wikipedia.org/wiki/Mandelbrot_set
// Calcul du nombre d'itérations pour la fractale de Mandelbrot
int mandelbrot(long double real, long double imaginary, int max_iterations){
    long double z_real = 0;
    long double z_imaginary = 0;
    int iterations = 0;
    while(z_real*z_real + z_imaginary*z_imaginary < 4 && iterations < max_iterations){
        long double z_real_temp = z_real*z_real - z_imaginary*z_imaginary + real;
        z_imaginary = 2*z_real*z_imaginary + imaginary;
        z_real = z_real_temp;
        iterations++;
    }
    return iterations;
}

// https://en.wikipedia.org/wiki/Julia_set
// Calcul du nombre d'itérations pour la fractale de Julia 
int julia(long double real, long double imaginary, int max_iterations, long double c_real, long double c_imaginary){
    long double z_real = real;
    long double z_imaginary = imaginary;
    int iterations = 0;
    while(z_real*z_real + z_imaginary*z_imaginary < 4 && iterations < max_iterations){
        long double z_real_temp = z_real*z_real - z_imaginary*z_imaginary + c_real;
        z_imaginary = 2*z_real*z_imaginary + c_imaginary;
        z_real = z_real_temp;
        iterations++;
    }
    return iterations;
}

// https://en.wikipedia.org/wiki/Burning_Ship_fractal
// Calcul du nombre d'itérations pour la fractale de Burning Ship
int burning_ship(long double real, long double imaginary, int max_iterations){
    long double z_real = 0;
    long double z_imaginary = 0;
    int iterations = 0;
    while(z_real*z_real + z_imaginary*z_imaginary < 4 && iterations < max_iterations){
        long double z_real_temp = z_real*z_real - z_imaginary*z_imaginary + real;
        z_imaginary = fabs(2*z_real*z_imaginary) + imaginary;
        z_real = z_real_temp;
        iterations++;
    }
    return iterations;
}
