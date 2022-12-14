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

    // Distribution des premières lignes à traiter aux workers
    for(int i = 1; i < world_size; i++){
        // Envoie la ligne à traiter au worker
        MPI_Send(&first_row, 1, MPI_INT, i, 0, comm);
        // Incrémenter la ligne à traiter
        first_row += range;
        // Affiche l'information envoyée
        //printf("Manager : Sending %d to worker %d\n",first_row,i);
    }

    int *received_data = (int *)malloc(1+(range)*parameters->width*3*sizeof(int));
    int starting_row_received, source;
    int amount_received = 0;
    int chunks_to_receive = parameters->height/parameters->chunks;
    MPI_Status status;

    while(amount_received < chunks_to_receive){
        // Réception des données
        MPI_Recv(received_data, 1+(range)*parameters->width*3, MPI_INT, MPI_ANY_SOURCE, 0, comm, &status);
        starting_row_received = received_data[0];
        source = status.MPI_SOURCE;

        // Affiche l'information reçue et la source
        //printf("Manager : Received %d from worker %d\n",starting_row_received,source);

        amount_received++;

        // On copie les données reçues dans le tableau pixels
        for(int i = 0; i < range; i++){
            for(int j = 0; j < parameters->width; j++){
                pixels[((starting_row_received+i)*parameters->width+j)].green = (uint8_t) received_data[1+(i*parameters->width+j)*3];
                pixels[((starting_row_received+i)*parameters->width+j)].red = (uint8_t) received_data[1+(i*parameters->width+j)*3+1];
                pixels[((starting_row_received+i)*parameters->width+j)].blue = (uint8_t) received_data[1+(i*parameters->width+j)*3+2];
            }
        }
        
        // S'il reste des lignes à traiter, on envoie au worker
        if(first_row < parameters->height){
            MPI_Send(&first_row, 1, MPI_INT, status.MPI_SOURCE, 0, comm);
            // Incrémenter la ligne à traiter
            first_row += range;
            // Affiche l'information envoyée
            //printf("Manager : Sending %d to worker %d\n",first_row,received_data[0]);
        }
        
    }

    // On affiche que tout a été reçu
    printf("Manager : All data received\n");

    int endval = -1;

    // On envoie -1 à tous les workers pour les arrêter
    for(int i = 1; i < world_size; i++){
        MPI_Send(&endval, 1, MPI_INT, i, 0, comm);
    }

    MPI_Barrier(comm);

    printf("Manager : All workers stopped\n");
    
    

    
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

    int range = parameters->chunks;     // Nombre de lignes traitées par le worker d'un coup
    int * send_data = (int *)malloc(1+range*parameters->width*3*sizeof(int)); // Tableau de pixels à envoyer au manager// Structure qui va stocker les valeurs RGB de chaque pixel
    bitmap_rgb * rgb = malloc(sizeof(bitmap_rgb)); // Structure qui va stocker les valeurs RGB de chaque pixel

    while(1){

        // Reçoit les informations nécessaires à la génération de l'image
        int starting_row;
        MPI_Recv(&starting_row, 1, MPI_INT, 0, 0, comm, MPI_STATUS_IGNORE);

        // Si la valeur reçue est -1, on arrête le worker
        if(starting_row == -1){
            printf("Worker %d : Stopping\n",world_rank);
            break;
        }

        // On stocke la ligne de départ dans le tableau pour le manager
        send_data[0] = starting_row;
        // Affiche les informations reçues
        //printf("Worker %d : Received %d and range is %d\n",world_rank,starting_row,range);

        // remplissage du tableau de pixels avec les valeurs de chaque pixel de la ligne starting_row à starting_row+range
        for(int i = 0; i < range; i++){
            double imaginary = parameters_calc->max_im - (i+starting_row) * parameters_calc->step;
            for(int j = 0; j < parameters->width; j++){
                // On calcule les coordonnées du pixel
                double real = parameters_calc->min_re + j * parameters_calc->step;

                // On calcule le nombre d'itérations
                int iterations = mandelbrot(real,imaginary,parameters->max_iterations);
                iterations_to_rgb(iterations,parameters->max_iterations,rgb);
                // Mettre du noir si le nombre d'itérations est égal au nombre maximum d'itérations sinon mettre du blanc
                /* if(iterations == parameters->max_iterations){
                    rgb->red = 0;
                    rgb->green = 0;
                    rgb->blue = 0;
                }else{
                    rgb->red = 255;
                    rgb->green = 255;
                    rgb->blue = 255;
                } */
                // affichage des coordonnées du pixel et du nombre d'itérations
                //printf("Worker %d : Calculating pixel (%d,%d) with coordinates (%f,%f), interations %d\n",world_rank,j,i+starting_row,real,imaginary,iterations);
                // On stocke les valeurs RGB dans le tableau de pixels
                send_data[1+(i*parameters->width+j)*3] = rgb->blue;
                send_data[1+(i*parameters->width+j)*3+1] = rgb->green;
                send_data[1+(i*parameters->width+j)*3+2] = rgb->red;
            }
        }

        // Envoie le tableau de pixels au manager
        MPI_Send(send_data, 1+range*parameters->width*3, MPI_INT, 0, 0, comm);
        
    }
    free(send_data);
    free(rgb);
    MPI_Barrier(comm);
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

int mandelbrot(double real, double imaginary, int max_iterations){
    double z_real = 0;
    double z_imaginary = 0;
    int iterations = 0;
    while(z_real*z_real + z_imaginary*z_imaginary < 4 && iterations < max_iterations){
        double z_real_temp = z_real*z_real - z_imaginary*z_imaginary + real;
        z_imaginary = 2*z_real*z_imaginary + imaginary;
        z_real = z_real_temp;
        iterations++;
    }
    return iterations;
}