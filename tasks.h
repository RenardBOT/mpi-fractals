#ifndef TASKS_H
#define TASKS_H

#include <mpi.h>
#include "parameters.h"
#include "save_bmp.h"


int manager_task(parameters_t * parameters, MPI_Comm comm, bitmap_rgb * pixels);
int worker_task(parameters_t * parameters, parameters_calc_t * parameters_calc, MPI_Comm comm);
int iterations_to_rgb(int iterations,int max_iterations,bitmap_rgb * rgb);
int mandelbrot(double real, double imaginary, int max_iterations);

#endif /* TASKS_H */
