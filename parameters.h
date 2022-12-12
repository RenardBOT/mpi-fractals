#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Paramètres par défaut
#define M_WIDTH             1250             // Largeur de l'image en pixels
#define M_HEIGHT            3080             // Hauteur de l'image en pixels
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
    modefract_t modefract;
} parameters_t;                                    

void parse_args(int argc, char** argv, parameters_t * parameters, int world_rank, int world_size); 
void init_parameters(parameters_t * parameters);  
void failure(char * error, int world_rank);                                              

#endif /* PARAMETERS_H */
