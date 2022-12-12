#include "tasks.h"

int manager_task(parameters_t *parameters, MPI_Comm comm, bitmap_rgb *pixels){

    // Sympatique petit message de bienvenue pour le manager :)
    printf("Hello world from the manager\n");
    int MPI_Barrier( MPI_Comm comm );

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

int worker_task(parameters_t * parameters, parameters_calc_t * parameters_calc, MPI_Comm comm){

    // Récupère les variables du commutateur
    int world_size,world_rank;
    MPI_Comm_size(comm, &world_size);         // Nombre de tâches
    MPI_Comm_rank(comm, &world_rank);         // Rang de la tâche

    // Sympatique petit message de bienvenue pour le worker :)
    printf("Hello world from processor rank %d out of %d processors\n",world_rank,world_size);
    int MPI_Barrier( MPI_Comm comm );

    // Reçoit les informations nécessaires à la génération de l'image
    int starting_row,range;
    range = parameters->chunks;
    MPI_Recv(&starting_row, 1, MPI_INT, 0, 0, comm, MPI_STATUS_IGNORE);

    // Un tableau de int dynamique qui va stocker les valeurs vert, bleu et rouge de chaque pixel de chunks lignes
    // plus une case contenant starting row 
    int * pixels = (int *)malloc(1+(range)*parameters->width*3*sizeof(int));
    // On stocke la ligne de départ dans le tableau pour le manager
    pixels[0] = starting_row;
    // Affiche les informations reçues
    printf("Worker %d : Received %d and range is %d\n",world_rank,starting_row,range);

    // remplissage du tableau de pixels avec les valeurs de chaque pixel de la ligne starting_row à starting_row+range
    for(int i = 0; i < range; i++){
        double imaginary = parameters_calc->max_im - (i+starting_row) * parameters_calc->step;
        for(int j = 0; j < parameters->width; j++){
            // On calcule les coordonnées du pixel
            double real = parameters_calc->min_re + j * parameters_calc->step;

            // affichage des coordonnées du pixel
            printf("Worker %d : Calculating pixel (%d,%d) with coordinates (%f,%f)\n",world_rank,j,i+starting_row,real,imaginary);
            
            /* // On calcule le nombre d'itérations
            int iterations = mandelbrot(x,y,parameters->max_iterations);
            // On stocke les valeurs de chaque pixel dans le tableau
            pixels[1+(i*parameters->width+j)*3] = iterations;
            pixels[1+(i*parameters->width+j)*3+1] = j;
            pixels[1+(i*parameters->width+j)*3+2] = i+starting_row; */
        }
    }



    

    return 0;
}

int iterations_to_rgb(int iterations,int max_iterations,bitmap_rgb * rgb){
    //scale iterations from 0-max_iterations to 0-360

    int hue = (int)(360.0*iterations/max_iterations);
    int chroma = 255;
    int x = chroma * (1 - abs((hue/60)%2 - 1));
    int m = 255 - chroma;
    int r,g,b;
    if(hue < 60){
        r = chroma;
        g = x;
        b = 0;
    }else if(hue < 120){
        r = x;
        g = chroma;
        b = 0;
    }else if(hue < 180){
        r = 0;
        g = chroma;
        b = x;
    }else if(hue < 240){
        r = 0;
        g = x;
        b = chroma;
    }else if(hue < 300){
        r = x;
        g = 0;
        b = chroma;
    }else{
        r = chroma;
        g = 0;
        b = x;
    }
    rgb->red = r + m;
    rgb->green = g + m;
    rgb->blue = b + m;
}