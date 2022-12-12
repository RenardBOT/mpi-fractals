#include "parameters.h"

// "w:h:i:c:x:y:m" 
void parse_args(int argc, char** argv, parameters_t * parameters, int world_rank, int world_size){
    int opt;
    opterr = 0;

    while ((opt = getopt(argc, argv, OPTSTR)) != EOF)
       switch(opt) {

            // Traitement des valeurs entières positives
            case 'w':
            case 'h':
            case 'i':
            case 'c':

                int argint = atoi(optarg);

                // Largeur et hauteur doivent être comprises entre deux valeurs (voir constantes)
                if(opt == 'w'){  
                    if(argint < atoi(M_MIN_WIDTH) || argint > atoi(M_MAX_WIDTH))
                        failure("-w doit être un entier compris entre "M_MIN_WIDTH" et "M_MAX_HEIGHT,world_rank);
                    parameters->width = argint;
                } else

                if(opt == 'h'){  
                    if(argint < atoi(M_MIN_WIDTH) || argint > atoi(M_MAX_HEIGHT))
                        failure("-h doit être un entier compris entre "M_MIN_WIDTH" et "M_MAX_HEIGHT,world_rank);
                    parameters->height = argint;
                } else

                // Le nombre d'itérations doit être strictement positif
                if(opt == 'i')  {  
                    if(argint < 0)
                        failure("-i doit être un entier strictement supérieur à 0",world_rank);
                    parameters->max_iterations = argint;
                } else

                // La taille d'un bloc de données (en nombre de lignes du tableau de pixels) doit être strictement supérieure à 0,
                // plus petite que le ratio entre la hauteur et le nombre de workers (afin que chaque worker aie au moins un bloc),
                // et être un diviseur de la hauteur (pour que chaque bloc ait la même taille en nombre de lignes)
                if(opt == 'c') {  
                    if(argint < 1 || argint > atoi(M_MAX_HEIGHT))
                        failure("-c doit être un entier compris entre 1 et "M_MAX_HEIGHT" (hauteur)",world_rank);
                                        if(atoi(M_MAX_HEIGHT) % argint != 0)
                        failure("-c doit être un diviseur de "M_MAX_HEIGHT" (hauteur)",world_rank);
                    parameters->chunks = argint;
                } else

            // Traitement des valeurs flottantes
            case 'x':
                parameters->origin_re = atoi(optarg);
                break;

            case 'y':
                parameters->origin_im = atoi(optarg);
                break;
            
            case 'm':
                if(strcmp(optarg,"0") == 0 || strcmp(optarg,"MANDELBROT") == 0)
                    parameters->modefract = 0;
                else
                if(strcmp(optarg,"1") == 0 || strcmp(optarg,"JULIA") == 0)
                    parameters->modefract = 1;
                else 
                    failure("-m vaut soit 0/MANDELBROT, soit 1/JULIA",world_rank); 

                
           
       }
}

void init_parameters(parameters_t *parameters){
    parameters->width           = M_WIDTH;
    parameters->height          = M_HEIGHT;
    parameters->max_iterations  = M_MAX_ITERATIONS;
    parameters->chunks          = M_CHUNKS;
    parameters->origin_re       = M_ORIGIN_RE;
    parameters->origin_im       = M_ORIGIN_IM;
    parameters->range           = M_RANGE;
    parameters->modefract       = M_MODEFRACT;
}
