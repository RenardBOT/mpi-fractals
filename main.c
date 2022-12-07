// 1 - Déclaration des librairies
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>


// 2 - Déclaration des constantes

// Paramètres par défaut
#define M_WIDTH             400             // Largeur de l'image en pixels
#define M_HEIGHT            400             // Hauteur de l'image en pixels
#define M_MAX_ITERATIONS    500             // Nombre d'itérations nécéssaires pour considérer la suite non-divergente
#define M_CHUNKS            1               // Nombre de ligne du tableau calculées par un Worker
#define M_ORIGIN_RE         -.75             // Origine du repère à afficher sur l'axe des réels (milieu de la fenêtre)
#define M_ORIGIN_IM         .0              // Origine du repère à afficher sur l'axe des immaginaires purs (milieu de la fenêtre)
#define M_RANGE             3
#define M_MODEFRACT         0               // Choix de la fractale : Julia (non implémenté) ou Mandelbrot

// Paramètres limites
#define M_MIN_WIDTH         "20"
#define M_MAX_WIDTH         "10000"
#define M_MIN_HEIGHT        "20"
#define M_MAX_HEIGHT        "10000"

#define OPTSTR              "w:h:i:c:x:y:m:"   // Liste des arguments acceptés en ligne de commande 
                                                // (Width,Height,Iterations,Chunks,Mode,Origin,)

// 3 - Déclaration externes
extern int errno;           // Variable stockant les codes d'erreur utilisé par la librairie standard 
extern char *optarg;        //
extern int opterr, optind;

// 4 - Typedefs

// Enumération des différents modes
typedef enum {
    MANDELBROTssqs = 0,sqsdz
    JULIA = 1
} modefract_t;

// Structure stockant la couleur d'un pixel
typedef struct rgb_t{
    double red;
    double green;
    double blue;
} rgb_t;

// Structure stockant un nombre complexe (real + i*imaginary)
typedef struct complex_t{
    double real;
    double imaginary;
} complex_t;

// Structure stockant les paramètres
typedef struct parameters_t{
    int width;
    int height;
    int max_iterations;
    int chunks;
    double origin_re;
    double origin_im;
    double range;
    modefract_t modefract;
} parameters_t;

// 6 - Variables globales

// 7 - Signatures des fonctions

void parse_args(int argc, char** argv, parameters_t * parameters, int world_rank, int world_size);
void print_parameters(parameters_t * parameters);
void failure(char * error, int world_rank);

// ---------------------------

int main(int argc, char** argv)
{
    parameters_t * parameters = (parameters_t *)malloc(sizeof(*parameters));
    parameters->width           = M_WIDTH;
    parameters->height          = M_WIDTH;
    parameters->max_iterations  = M_MAX_ITERATIONS;
    parameters->chunks          = M_CHUNKS;
    parameters->origin_re       = M_ORIGIN_RE;
    parameters->origin_im       = M_ORIGIN_IM;
    parameters->range           = M_RANGE;
    parameters->modefract       = M_MODEFRACT;

    
    
    MPI_Init(NULL, NULL);

    int world_size,world_rank,name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];        
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);         // Nombre de tâches
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);         // Rang de la tâche
    MPI_Get_processor_name(processor_name, &name_len);  // Nom et taille du nom du noeud

    
    parse_args(argc,argv,parameters,world_rank,world_size);
    if(world_rank == 0)
        print_parameters(parameters) ;

    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;

    
}

// 8 - Déclaration des fonctions 

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
    
void print_parameters(parameters_t * parameters){
    printf(" --- PARAMETRES ---------\n\n");
    printf("Dimensions : %dpx * %dpx\n",parameters->width,parameters->height);
    printf("Origine : %f + i*%f\n",parameters->origin_re,parameters->origin_im);
    printf("Intervalle réel : %f\n",parameters->range);
    printf("Nombre d'itérations : %d\n",parameters->max_iterations);
    printf("Taille d'un bloc de données : %d\n\n",parameters->chunks);

    switch(parameters->modefract){
        case MANDELBROT:
            printf("ENSEMBLE CHOISI : Mandelbrot\n");
            break;

        case JULIA:
            printf("ENSEMBLE CHOISI : Julia\n");
            break;
    }
    printf("-------------\n");
}

void failure(char * error, int world_rank){
    if(world_rank == 0)
        printf("\033[91;1mErreur : \033[0m%s\n\nSortie forcée du programme\n",error);
    exit(EXIT_FAILURE);
};


