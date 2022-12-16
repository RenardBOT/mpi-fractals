#include "tasks.h"

void manager_task(parameters_t *parameters, MPI_Comm comm, bitmap_rgb *pixels){

    

    // Récupère les variables du commutateur
    int world_size,world_rank,name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Comm_size(comm, &world_size);         // Nombre de tâches
    MPI_Comm_rank(comm, &world_rank);         // Rang de la tâche
    MPI_Get_processor_name(processor_name, &name_len);  // Nom et taille du nom du noeud

    // Le manager annonce sa présence et attend que les autres tâches soient prêtes
    printf("[ ] \033[1mManager :\033[0m Tache %d sur %d démarrée sur le noeud %s\n", world_rank, world_size, processor_name);
    MPI_Barrier(comm);

    int first_row = 0;                  // Indice de la ligne à traiter par les workers 
    int range = parameters->chunks;     // Nombre de lignes traitées par le worker d'un coup

    // Distribution des premières lignes à traiter aux workers
    for(int i = 1; i < world_size; i++){
        MPI_Send(&first_row, 1, MPI_INT, i, 0, comm);   // Envoie la ligne à traiter au worker i
        first_row += range;                             // Incrémente la ligne à traiter
    }

    // Allocation de la mémoire pour les données reçues par le manager et vérification de l'allocation
    int *received_data = (int *)malloc(1+(range)*parameters->width*3*sizeof(int));  
    if(received_data == NULL){
        printf("Erreur : Echec de l'allocation de mémoire pour les données reçues\n");
        exit(1);
    }

    int starting_row_received;                             // Indice de la ligne reçue et source du message
    int amount_received = 0;                                        // Nombre de lignes reçues par le manager (pour savoir quand il a fini)
    int chunks_to_receive = parameters->height/parameters->chunks;  // Nombre de lignes à recevoir
    MPI_Status status;                                              // Status du message reçu

    while(amount_received < chunks_to_receive){
        // Réception des données
        MPI_Recv(received_data, 1+(range)*parameters->width*3, MPI_INT, MPI_ANY_SOURCE, 0, comm, &status);
        starting_row_received = received_data[0];

        amount_received++;  // Incrémente le nombre de lignes reçues

        // On copie les données reçues dans le tableau pixels
        for(int i = 0; i < range; i++){
            for(int j = 0; j < parameters->width; j++){
                pixels[((starting_row_received+i)*parameters->width+j)].green = (uint8_t) received_data[1+(i*parameters->width+j)*3];
                pixels[((starting_row_received+i)*parameters->width+j)].red = (uint8_t) received_data[1+(i*parameters->width+j)*3+1];
                pixels[((starting_row_received+i)*parameters->width+j)].blue = (uint8_t) received_data[1+(i*parameters->width+j)*3+2];
            }
        }
        
        // S'il reste des lignes à traiter, on envoie au worker qui vient de finir (au boulot!)
        if(first_row < parameters->height){
            MPI_Send(&first_row, 1, MPI_INT, status.MPI_SOURCE, 0, comm);
            first_row += range; // Incrémenter la ligne à traiter
        }

        // Affichage d'une barre de progression en fonction du pourcentage de données reçues
        printf("[ ] \033[1mManager :\033[0m Avancement de la réception [");
        for(int i = 0; i < (int) (amount_received*100.0/chunks_to_receive)/2; i++){
            printf("=");
        }
        for(int i = 0; i < 50-(int) (amount_received*100.0/chunks_to_receive)/2; i++){
            printf(" ");
        }
        printf("]\r");


    }
    printf("\n");
    printf("[ ] \033[1mManager :\033[0m 100%% des données reçues\n");

    // Le manager a fini de recevoir les données
    printf("[ ] \033[1mManager :\033[0m Toutes les données ont été reçues!\n");

    // On envoie -1 à tous les workers pour les arrêter
    int endval = -1;
    for(int i = 1; i < world_size; i++){
        MPI_Send(&endval, 1, MPI_INT, i, 0, comm);
    }

    free(received_data);

    // Barrière permettant au manager d'attendre que les workers aient bien reçu le message d'arrêt
    MPI_Barrier(comm);
    printf("[ ] \033[1mManager :\033[0m Tous les workers sont arrêtés, début de la phase d'écriture du fichier\n");

    
}

void worker_task(parameters_t * parameters, parameters_calc_t * parameters_calc, MPI_Comm comm){

    // Récupère les variables du commutateur
    int world_size,world_rank,name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Comm_size(comm, &world_size);         // Nombre de tâches
    MPI_Comm_rank(comm, &world_rank);         // Rang de la tâche
    MPI_Get_processor_name(processor_name, &name_len);  // Nom et taille du nom du noeud

    // Le worker annonce sa présence et attend que les autres tâches soient prêtes
    printf("< >\033[1m Worker  :\033[0m Tache %d sur %d démarrée sur le noeud %s\n", world_rank, world_size, processor_name);
    MPI_Barrier( comm );

    int iterations = 0;
    int range = parameters->chunks;     // Nombre de lignes traitées par le worker d'un coup

    // Allocation de la mémoire pour les données reçues du manager et vérification de l'allocation
    int * send_data = (int *)malloc(1+range*parameters->width*3*sizeof(int)); 
    if(send_data == NULL){
        printf("Erreur : Echec de l'allocation de mémoire pour les données reçues par le worker %d\n",world_rank);
        exit(1);
    }

    // Allocation de la mémoire pour contenir les valeurs RGB des pixels et vérification de l'allocation
    bitmap_rgb * rgb = malloc(sizeof(bitmap_rgb)); 
    if(rgb == NULL){
        printf("Erreur : Echec de l'allocation de mémoire pour les données RGB du worker %d\n",world_rank);
        exit(1);
    }

    while(1){

        // Reçoit les informations nécessaires à la génération de l'image
        int starting_row;
        MPI_Recv(&starting_row, 1, MPI_INT, 0, 0, comm, MPI_STATUS_IGNORE);

        // Si la valeur reçue est -1, on arrête le worker
        if(starting_row == -1){
            break;
        }

        // La première case du buffer contient la première ligne à générer
        send_data[0] = starting_row;

        // Remplissage du tableau de pixels avec les valeurs de chaque pixel de la ligne starting_row à starting_row+range
        for(int i = 0; i < range; i++){
            double imaginary = parameters_calc->max_im - (i+starting_row) * parameters_calc->step;
            for(int j = 0; j < parameters->width; j++){
                // On calcule les coordonnées du pixel
                double real = parameters_calc->min_re + j * parameters_calc->step;

                // On calcule le nombre d'itérations en fonction du mode de fractale (Mandelbrot ou Julia)
                if(parameters->modefract == MANDELBROT)
                    iterations = mandelbrot(real,imaginary,parameters->max_iterations);
                else
                    iterations = julia(real,imaginary,parameters->max_iterations,parameters->julia_re,parameters->julia_im);

                // On calcule les valeurs RGB en fonction du nombre d'itérations (noir si max_iterations atteint)
                if(iterations == parameters->max_iterations){
                    rgb->red = 0;
                    rgb->green = 0;
                    rgb->blue = 0;
                }
                else{
                    hue2rgb((float)(iterations%parameters->max_palette)/parameters->max_palette,rgb);
                }

                // Stockage des valeurs RGB dans le tableau de pixels à envoyer au manager
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

    // Barrière permettant au worker d'attendre que le manager ait bien reçu le message d'arrêt
    MPI_Barrier(comm);
    printf("< >\033[1m Worker  :\033[0m Tâche %d arrêtée\n",world_rank);
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

int julia(double real, double imaginary, int max_iterations, double c_real, double c_imaginary){
    double z_real = real;
    double z_imaginary = imaginary;
    int iterations = 0;
    while(z_real*z_real + z_imaginary*z_imaginary < 4 && iterations < max_iterations){
        double z_real_temp = z_real*z_real - z_imaginary*z_imaginary + c_real;
        z_imaginary = 2*z_real*z_imaginary + c_imaginary;
        z_real = z_real_temp;
        iterations++;
    }
    return iterations;
}