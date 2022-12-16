#ifndef TASKS_H
#define TASKS_H

#include <mpi.h>

#include "parameters.h"
#include "save_bmp.h"
#include "palette.h"
#include "fractals.h"



void manager_task(parameters_t * parameters, MPI_Comm comm, bitmap_rgb * pixels);
void worker_task(parameters_t * parameters, parameters_calc_t * parameters_calc, MPI_Comm comm);

#endif /* TASKS_H */
