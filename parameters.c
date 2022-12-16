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
    int flag_settings = 0;
    int flag_julia = 0;
    struct option longopts[] = {
    { "help",    no_argument, &flag_help,    1   },
    { "settings",no_argument, &flag_settings,1   },
    { "julia",   no_argument, &flag_julia,   1   },
    { 0, 0, 0, 0 }
    };
    int opt;
    opterr = 0;

    float argfloat;
    int argint;
    

    while ((opt = getopt_long(argc, argv, OPTSTR,longopts,NULL)) != EOF)
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
                    break;
                } else

                if(opt == 'k')  {  
                    parameters->julia_im = -argfloat; // Inversion de l'axe des ordonnées car les images BMP scannent de bas en haut
                    break;
                } 

            // Traitement des booléens
            case 's':
                parameters->print_parameters = 1;
                break;
    }
    if(flag_help == 1){
        print_help();
        exit(0);
    }
    if(flag_settings == 1){
        parameters->print_parameters = 1;
    }
    if(flag_julia == 1){
        parameters->modefract = JULIA;
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
    parameters->print_parameters= M_PRINT_PARAMETERS;
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
    printf(" --- PARAMETRES ---------\n\n");
    printf("Dimensions : %dpx * %dpx\n",parameters->width,parameters->height);
    printf("Origine : %f + i*%f\n",parameters->origin_re,parameters->origin_im);
    printf("Intervalle réel : %f\n",parameters->range);
    printf("Nombre d'itérations : %d\n",parameters->max_iterations);
    printf("Taille d'un bloc de données : %d\n\n",parameters->chunks);
    printf("Nombre de couleurs : %d\n",parameters->max_palette);

    switch(parameters->modefract){
        case MANDELBROT:
            printf("ENSEMBLE CHOISI : Mandelbrot\n");
            break;

        case JULIA:
            printf("ENSEMBLE CHOISI : Julia\n");
            printf("Constante de Julia : %f + i*%f\n",parameters->julia_re,parameters->julia_im);
            break;
    }
    printf("\n\n");
}

void print_parameters_calc(parameters_calc_t * parameters_calc){
    printf(" --- PARAMETRES CALCULS ---------\n\n");
    printf("Pas réel : %f\n",parameters_calc->step);
    printf("Min réel : %f\n",parameters_calc->min_re);
    printf("Max réel : %f\n",parameters_calc->max_re);
    printf("Min imaginaire : %f\n",parameters_calc->min_im);
    printf("Max imaginaire : %f\n",parameters_calc->max_im);
    printf("\n\n");
}

// print en gras
void print_bold(char *str){
    printf("\033[1m%s\033[0m",str);
}
void print_help(){
    printf(" --- AIDE ---------\n\n");
    printf("Utilisation : ./fractale [OPTIONS]\n\n");
    printf("OPTIONS :\n");
    printf("\033[1m--help :\033[0m Affiche l'aide\n");
    printf("\033[1m--settings :\033[0m Affiche les paramètres\n");
    printf("\033[1m-w :\033[0m Largeur de l'image (en pixels)\n");
    printf("\033[1m-h :\033[0m Hauteur de l'image (en pixels)\n");
    printf("\033[1m-i :\033[0m Nombre d'itérations pour considérer la suite divergente\n");
    printf("\033[1m-c :\033[0m Taille d'un bloc de données, c'est à dire le nombre de l'image à traiter d'un coup pour un worker\n");
    printf("\033[1m-r :\033[0m Intervalle de l'espace solution sur l'axe de réels (niveau de zoom)\n");
    printf("\033[1m-o :\033[0m Origine (x + i*y)\n");
    printf("\033[1m-m :\033[0m Mode de fractale (0/MANDELBROT, 1/JULIA)\n");
    printf("\033[1m-j :\033[0m Constante de Julia (x + i*y)\n");
    printf("\033[1m-p :\033[0m Nombre de couleurs\n");
    printf("\033[1m-s :\033[0m Affiche les paramètres\n");

    
    printf("\n\n");
}