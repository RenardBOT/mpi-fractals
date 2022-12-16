#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Paramètres par défaut
#define M_WIDTH             1000            // Largeur de l'image en pixels
#define M_HEIGHT            1000            // Hauteur de l'image en pixels
#define M_MAX_ITERATIONS    250             // Nombre d'itérations nécéssaires pour considérer la suite non-divergente
#define M_CHUNKS            1               // Nombre de ligne du tableau calculées par un Worker
#define M_ORIGIN_RE         -.75            // Origine du repère à afficher sur l'axe des réels (milieu de la fenêtre)
#define M_ORIGIN_IM         .0              // Origine du repère à afficher sur l'axe des immaginaires purs (milieu de la fenêtre)
#define M_RANGE             3               // Intervalle de valeurs à afficher sur l'axe des réels
#define M_MAX_PALETTE       100             // Limite des couleurs de la palette
#define M_MODEFRACT         0               // Choix de la fractale : Julia (non implémenté) ou Mandelbrot
#define M_JULIA_RE          -.8             // Partie réelle du nombre complexe pour la fractale de Julia
#define M_JULIA_IM          .156            // Partie imaginaire du nombre complexe pour la fractale de Julia
#define M_PRINT_PARAMETERS  0               // Affichage des paramètres en sortie standard
#define M_PRINT_HELP        0               // Affichage de l'aide en sortie standard


// Paramètres limites
#define M_MIN_WIDTH         "20"
#define M_MAX_WIDTH         "10000"
#define M_MIN_HEIGHT        "20"
#define M_MAX_HEIGHT        "10000"

#define OPTSTR              "w:h:i:c:x:y:r:p:j:k:"   // Liste des arguments acceptés en ligne de commande 
                                                        // (Width,Height,Iterations,Chunks,Mode,Origin,Mode,Range,Julia,Settings)

// 3 - Déclaration externes
extern int errno;           // Variable stockant les codes d'erreur utilisé par la librairie standard 
extern char *optarg;        // Variable stockant les arguments des options
extern int opterr, optind;  // Variables utilisées par getopt (récupération des arguments)


// Enumération des différents modes
typedef enum {
    MANDELBROT = 0, 
    JULIA = 1
} modefract_t;

// Structure stockant les paramètres
typedef struct{
    int width;
    int height;
    int max_iterations;
    int chunks;
    double origin_re;
    double origin_im;
    double range;
    int max_palette;
    double julia_re;
    double julia_im;
    int print_parameters;
    modefract_t modefract;
} parameters_t;

// Structure stockant la façon dont les paramètres sont exploités pour les calculs
typedef struct{
    double step;
    double min_re;
    double max_re;
    double min_im;
    double max_im;
} parameters_calc_t;

void parse_args(int argc, char** argv, parameters_t * parameters, int world_rank, int world_size); 
void init_parameters(parameters_t * parameters);
void init_parameters_calc(parameters_t * parameters, parameters_calc_t * parameters_calc);  
void failure(char * error, int world_rank);
void print_parameters(parameters_t * parameters);
void print_parameters_calc(parameters_calc_t * parameters_calc);
void print_help();

#endif /* PARAMETERS_H */
