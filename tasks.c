#include "tasks.h"

int manager_task(parameters_t *parameters, MPI_Comm comm, bitmap_rgb *pixels){

    // Sympatique petit message de bienvenue pour le manager :)
    printf("Hello world from the manager\n");

    // Récupère les variables du commutateur
    int world_size,world_rank;
    MPI_Comm_size(comm, &world_size);         // Nombre de tâches
    MPI_Comm_rank(comm, &world_rank);         // Rang de la tâche

    int first_row = 0;                  // Indice de la ligne à traiter par le worker
    int range = parameters->chunks;     // Nombre de lignes traitées par le worker d'un coup

    // Distribution des lignes à traiter aux workers
    for(int i = 1; i < world_size; i++){
        // Envoie la ligne à traiter au worker
        MPI_Send(&first_row, 1, MPI_INT, i, 0, comm);
        // Incrémenter la ligne à traiter
        first_row += range;
        // Affiche l'information envoyée
        printf("Manager : Sending %d to worker %d\n",first_row,i);
    }

    return 0;
}

int worker_task(parameters_t * parameters, MPI_Comm comm){

    
    // Récupère les variables du commutateur
    int world_size,world_rank;
    MPI_Comm_size(comm, &world_size);         // Nombre de tâches
    MPI_Comm_rank(comm, &world_rank);         // Rang de la tâche

    // Sympatique petit message de bienvenue pour le worker :)
    printf("Hello world from processor rank %d out of %d processors\n",world_rank,world_size);

    // Reçoit les informations nécessaires à la génération de l'image
    int starting_row,range;
    range = parameters->chunks;
    MPI_Recv(&starting_row, 1, MPI_INT, 0, 0, comm, MPI_STATUS_IGNORE);


    // Affiche les informations reçues
    printf("Worker %d : Received %d and range is %d\n",world_rank,starting_row,range);
    

    return 0;
}