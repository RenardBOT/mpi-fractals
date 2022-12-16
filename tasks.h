#ifndef TASKS_H
#define TASKS_H

#include <mpi.h>
#include "parameters.h"
#include "save_bmp.h"
#include "palette.h"


void manager_task(parameters_t * parameters, MPI_Comm comm, bitmap_rgb * pixels);
void worker_task(parameters_t * parameters, parameters_calc_t * parameters_calc, MPI_Comm comm);
int mandelbrot(double real, double imaginary, int max_iterations);
int julia(double real, double imaginary, int max_iterations, double c_real, double c_imaginary);

#endif /* TASKS_H */
