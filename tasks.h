#ifndef TASKS_H
#define TASKS_H

#include <mpi.h>
#include "parameters.h"
#include "save_bmp.h"


int manager_task(parameters_t * parameters, MPI_Comm comm, bitmap_rgb * pixels);
int worker_task(parameters_t * parameters, MPI_Comm comm);

#endif /* TASKS_H */
