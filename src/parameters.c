#include "parameters.h"

/*  
    Traitement des arguments entrés en ligne de commande.
    -w : largeur de l'image
    -h : hauteur de l'image
    -i : nombre d'itérations
    -c : nombre de couleurs
    -x : origine du repère sur l'axe des réels
    -y : origine du repère sur l'axe des imaginaires purs
    -j : partie réelle du nombre complexe pour la fractale de Julia
    -k : partie imaginaire du nombre complexe pour la fractale de Julia
    -r : intervalle de valeurs à afficher sur l'axe des réels (niveau de zoom)
    -p : taille de la palette de couleurs
*/
void parse_args(int argc, char** argv, parameters_t * parameters, int world_rank, int world_size){

    // Drapeaux à lever quand des longs arguments sont croisés (help, settings, etc...)
    int flag_help = 0;
    int flag_verbose = 0;
    int flag_julia = 0;
    int flag_bship = 0;
    int flag_prog = 0;
    struct option longopts[] = {
    { "help",    no_argument,       &flag_help,     1   },
    { "verbose", no_argument,       &flag_verbose,  1   },
    { "julia",   no_argument,       &flag_julia,    1   },
    { "bship",   no_argument,       &flag_bship,    1   },
    { "prog",    no_argument,       &flag_prog,     1   },
    { "file",    optional_argument, NULL,           'f' },
    { 0, 0, 0, 0 }
    };
    int option_index = 0;
    int opt;
    opterr = 0;

    float argfloat;
    int argint;
    

    while ((opt = getopt_long(argc, argv, OPTSTR,longopts,&option_index)) != EOF)
       switch(opt) {

            // Traitement des valeurs entières positives
            case 'w':
            case 'h':
            case 'i':
            case 'c':
            case 'p':

                argint = atoi(optarg);

                // Largeur et hauteur doivent être comprises entre deux valeurs (voir constantes)
                if(opt == 'w'){  
                    if(argint < atoi(M_MIN_WIDTH) || argint > atoi(M_MAX_WIDTH))
                        failure("-w doit être un entier compris entre "M_MIN_WIDTH" et "M_MAX_HEIGHT,world_rank);
                    parameters->width = argint;
                    break;
                } else

                if(opt == 'h'){  
                    if(argint < atoi(M_MIN_WIDTH) || argint > atoi(M_MAX_HEIGHT))
                        failure("-h doit être un entier compris entre "M_MIN_WIDTH" et "M_MAX_HEIGHT,world_rank);
                    parameters->height = argint;
                    break;
                } else

                // Le nombre d'itérations doit être strictement positif
                if(opt == 'i')  {  
                    if(argint < 0)
                        failure("-i doit être un entier strictement supérieur à 0",world_rank);
                    parameters->max_iterations = argint;
                    break;
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
                    break;
                } else

                if(opt == 'p') {  
                    if(argint < 0)
                        failure("-p doit être un entier strictement supérieur à 0",world_rank);
                    parameters->max_palette = argint;
                    break;
                } else

            // Traitement des valeurs flottantes
            case 'x':
            case 'y':
            case 'r':
            case 'j':
            case 'k':
                argfloat = atof(optarg);

                if(opt == 'x'){
                    parameters->origin_re = argfloat;
                    break;
                } else

                if(opt == 'y'){
                    parameters->origin_im = -argfloat; // Inversion de l'axe des ordonnées car les images BMP scannent de bas en haut
                    break;
                } else

                if(opt == 'r')  {  
                    if(argfloat < 0)
                        failure("-r doit être un flottant strictement supérieur à 0",world_rank);
                    parameters->range = argfloat;
                    break;
                } else

                if(opt == 'j')  {  
                    parameters->julia_re = argfloat;
                    printf("HERE\n");
                    break;
                } else

                if(opt == 'k')  {  
                    parameters->julia_im = -argfloat; // Inversion de l'axe des ordonnées car les images BMP scannent de bas en haut
                    break;
                } 

            // Traitement des strings
            case 'f':
                char *filename = optarg;
                // Verification que l'agument est un nom de fichier valide
                if (filename == NULL) {
                    failure("Option -f doit être suivi d'un nom de fichier",world_rank);
                }
                parameters->filename = optarg;
                break;
    }
    if(flag_help == 1){
        if(world_rank == 0)
            print_help();
        MPI_Finalize();
        exit(0);
    }
    if(flag_verbose == 1){
        parameters->verbose = 1;
    }
    if(flag_julia == 1 && flag_bship == 1){
        failure("Les options --julia et --bship sont mutuellement exclusives",world_rank);
    }
    if(flag_julia == 1){
        parameters->modefract = JULIA;
    }
    if(flag_bship == 1){
        parameters->modefract = BURNINGSHIP;
    }
    if(flag_prog == 1){
        parameters->progression = 1;
    }


}

// Initialisation des paramètres par défaut avec les constantes définies dans parameters.h
void init_parameters(parameters_t *parameters){
    parameters->width           = M_WIDTH;
    parameters->height          = M_HEIGHT;
    parameters->max_iterations  = M_MAX_ITERATIONS;
    parameters->chunks          = M_CHUNKS;
    parameters->origin_re       = M_ORIGIN_RE;
    parameters->origin_im       = M_ORIGIN_IM;
    parameters->range           = M_RANGE;
    parameters->julia_re        = M_JULIA_RE;
    parameters->julia_im        = M_JULIA_IM;
    parameters->max_palette     = M_MAX_PALETTE;
    parameters->modefract       = M_MODEFRACT;
    parameters->verbose         = M_VERBOSE;
    parameters->filename        = M_FILENAME;
    parameters->progression     = M_PROGRESSION;
}

// Calculs en amont des valeurs utilisées plusieurs fois par les workers 
void init_parameters_calc(parameters_t *parameters, parameters_calc_t *parameters_calc){
    parameters_calc->step = parameters->range / (parameters->width - 1);  // Distance réelle et imaginaire entre deux pixels
    parameters_calc->min_re = parameters->origin_re - parameters->range / 2;    // Coordonnées réelles des pixels les plus à gauche
    parameters_calc->max_re = parameters_calc->min_re + parameters_calc->step * (parameters->width - 1); // Coordonnées réelles des pixels les plus à droite
    parameters_calc->max_im = parameters->origin_im + parameters_calc->step * (parameters->height)/2 ; // Coordonnées réelles des pixels le plus en haut
    parameters_calc->min_im = parameters_calc->max_im - parameters_calc->step * (parameters->height - 1); // Coordonnées réelles des pixels le plus en bas
}

// Affichage des paramètres pour debug
void print_parameters(parameters_t * parameters){
    printf("   .____________________.\n");
    printf("___| PARAMETRES DE BASE |__________\n\n");
    printf("| Dimensions : %dpx * %dpx\n",parameters->width,parameters->height);
    printf("| Origine : %Lf + i*%Lf\n",parameters->origin_re,parameters->origin_im);
    printf("| Intervalle réel : %Lf\n",parameters->range);
    printf("| Nombre d'itérations : %d\n",parameters->max_iterations);
    printf("|Taille d'un bloc de données : %d\n\n",parameters->chunks);
    printf("| Nombre de couleurs : %d\n",parameters->max_palette);
    printf("| Nom du fichier : %s\n",parameters->filename);

    switch(parameters->modefract){
        case MANDELBROT:
            printf("ENSEMBLE CHOISI : Mandelbrot\n");
            break;

        case JULIA:
            printf("ENSEMBLE CHOISI : Julia\n");
            printf("Constante de Julia : %Lf + i*%Lf\n",parameters->julia_re,parameters->julia_im);
            break;

        case BURNINGSHIP:
            printf("ENSEMBLE CHOISI : Burning Ship\n");
            break;
    }
    printf("\n\n");
}

void print_parameters_calc(parameters_calc_t * parameters_calc){
    printf("   .____________________.\n");
    printf("___| PARAMETRES CALCULS |__________\n\n");
    printf("| Pas réel : %f\n",parameters_calc->step);
    printf("| Min réel : %f\n",parameters_calc->min_re);
    printf("| Max réel : %f\n",parameters_calc->max_re);
    printf("| Min imaginaire : %f\n",parameters_calc->min_im);
    printf("| Max imaginaire : %f\n",parameters_calc->max_im);
    printf("\n\n");
}


// Affichage de l'aide dans le terminal
void print_help(){
    printf(" --- AIDE ---------\n\n");
    printf("Utilisation : mpiexec -np <nb_noeuds> bin/mandelbrot [OPTIONS]\n\n");

    printf("OPTIONS :\n");
    printf("\033[1m--help :\033[0m Affiche l'aide\n");
    printf("\033[1m--verbose :\033[0m Affiche le déroulement du programme et paramètres\n");
    printf("\033[1m--prog :\033[0m Affiche une barre de progression\n");
    printf("\033[1m--julia :\033[0m Affiche les fractales de Julia\n");
    printf("\033[1m--bship :\033[0m Affiche la fractale Burning Ship\n");
    printf("\033[1m--file=<filename> :\033[0m Enregistre l'image dans le fichier <filename>.bmp du répertoire output\n");
    printf("\033[1m-w -h :\033[0m Résolution de l'image w*h\n");
    printf("\033[1m-i :\033[0m Nombre d'itérations pour considérer la suite divergente\n");
    printf("\033[1m-c :\033[0m Taille d'un bloc de données, c'est à dire le nombre de lignes de l'image à traiter d'un coup pour un worker\n");
    printf("\033[1m-r :\033[0m Intervalle de l'espace solution sur l'axe de réels (niveau de zoom)\n");
    printf("\033[1m-x -y :\033[0m Origine complexe (x + i*y)\n");
    printf("\033[1m-j -k :\033[0m Constante de Julia (x + i*y)\n");
    printf("\033[1m-p :\033[0m Nombre de couleurs, largeur de la palette\n");

    printf("VALEURS PAR DEFAUT :\n");
    printf("\033[1m--help :\033[0m 0\n");
    printf("\033[1m--verbose :\033[0m 0\n");
    printf("\033[1m--prog :\033[0m 0\n");
    printf("\033[1m--julia :\033[0m 0\n");
    printf("\033[1m--bship :\033[0m 0\n");
    printf("\033[1m--file=<filename> :\033[0m <julia|mandelbrot|bship>_timestamp.bmp\n");
    printf("\033[1m-w -h :\033[0m %dpx * %dpx\n",M_WIDTH,M_HEIGHT);
    printf("\033[1m-i :\033[0m %d\n",M_MAX_ITERATIONS);
    printf("\033[1m-c :\033[0m %d\n",M_CHUNKS);
    printf("\033[1m-r :\033[0m %d\n",M_RANGE);
    printf("\033[1m-x -y :\033[0m %f + i*%f\n",M_ORIGIN_RE,M_ORIGIN_IM);
    printf("\033[1m-j -k :\033[0m %f + i*%f\n",M_JULIA_RE,M_JULIA_IM);
    printf("\033[1m-p :\033[0m %d\n",M_MAX_PALETTE);
    printf("\n\n");
}

// Affiche un message d'erreur en rouge si le rang de la tâche est 0
void failure(char * error, int world_rank){
    if(world_rank == 0)
        printf("\033[91;1mErreur : \033[0m%s\n\nSortie forcée du programme\n",error);
    MPI_Finalize();
    exit(EXIT_FAILURE);
};
