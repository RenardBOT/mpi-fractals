// Déclaration des librairies et entêtes

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "parameters.h"
#include "save_bmp.h"
#include "tasks.h"



// Signatures des fonctions

int main(int argc, char **argv);
void write_file(parameters_t * parameters, bitmap_rgb * pixels);


// Déclaration des fonctions

int main(int argc, char** argv)
{
    MPI_Init(NULL, NULL);
    double start = MPI_Wtime();           // Début du chronomètre

    parameters_t * parameters   = (parameters_t *)malloc(sizeof(*parameters));
    parameters_calc_t * parameters_calc = (parameters_calc_t *)malloc(sizeof(*parameters_calc));
    if(parameters == NULL){
        printf("Erreur : Echec de l'allocation de mémoire pour les paramètres\n");
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
    if(parameters_calc == NULL){
        printf("Erreur : Echec de l'allocation de mémoire pour les paramètres de calcul\n");
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
    
    

    int world_size,world_rank,name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];        
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);         // Nombre de tâches
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);         // Rang de la tâche
    MPI_Get_processor_name(processor_name, &name_len);  // Nom et taille du nom du noeud

    // Vérification du nombre de tâches qui doit être supérieur ou égal à 2
    if(world_size < 2){
        if(world_rank == 0){
            printf("Erreur : Le nombre de tâches doit être supérieur ou égal à 2\n");
        }
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    init_parameters(parameters);
    parse_args(argc,argv,parameters,world_rank,world_size);
    init_parameters_calc(parameters,parameters_calc);

    

    

    if(world_rank == 0){
        // Allocation de la mémoire pour les pixels de l'image à écrire dans le fichier bmp
        bitmap_rgb * pixels = (bitmap_rgb *)malloc(parameters->width*parameters->height*sizeof(bitmap_rgb));
        if(pixels == NULL){
            printf("Erreur : Echec de l'allocation de mémoire pour les pixels\n");
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }

        if(parameters->verbose){
            print_parameters(parameters);
            print_parameters_calc(parameters_calc);
            printf("[ ] \033[1mManager :\033[0m Initialisation terminée, lancement des tâches\n");
        }

        MPI_Barrier(MPI_COMM_WORLD);

        manager_task(parameters,MPI_COMM_WORLD,pixels);
        write_file(parameters,pixels);
        free(pixels);
        if(parameters->verbose)
            printf("[ ] \033[1mManager :\033[0m Execution terminée, sortie du programme\n");
        
    }
    else{
        MPI_Barrier(MPI_COMM_WORLD);
        worker_task(parameters,parameters_calc,MPI_COMM_WORLD);
    }

    // Finalize the MPI environment.
    double end = MPI_Wtime(); // Fin du chronomètre

    if(world_rank == 0 && parameters->verbose){
        printf("\n[ ] \033[1mManager :\033[0m Temps d'exécution de %f secondes\n",end-start);
    }

    free(parameters);
    free(parameters_calc);

    MPI_Finalize();

    return 0;

    
}

// Définission de l'entête du fichier BMP et écriture du fichier
void write_file(parameters_t * parameters, bitmap_rgb * pixels){

    if(parameters->verbose)
        printf("[ ] \033[1mManager :\033[0m Ecriture de l'entête du fichier BMP\n");

    bitmap_file_header file_header;
    bitmap_info_header info_header;

    // La taille d'une ligne doit être un multiple de 4, sinon on ajoute des octets de padding
    int line_size_bytes = parameters->width*sizeof(bitmap_rgb); // Taille d'une ligne en octets
    if(line_size_bytes%4 != 0)
        line_size_bytes += 4 - line_size_bytes%4;

    file_header.type = 0x4D42;  // 'BM'
    file_header.size = sizeof(file_header) + sizeof(info_header) + line_size_bytes*parameters->height;  // Taille du fichier en octets
    file_header.reserved = 0;   // Réservé
    file_header.offset = sizeof(file_header) + sizeof(info_header); // Offset de l'image (taille de l'entête du fichier)

    info_header.size = sizeof(info_header);     // Taille de l'entête d'information
    info_header.width_px = parameters->width;   // Largeur de l'image en pixels
    info_header.height_px = parameters->height; // Hauteur de l'image en pixels
    info_header.planes = 1;                     // Nombre de plan, toujours 1 pour les images BMP
    info_header.bits_per_pixel = sizeof(bitmap_rgb)*8;  // Nombre de bits par pixel
    info_header.compression = 0;                // Compression, 0 pour aucune
    info_header.image_size_bytes = parameters->height*line_size_bytes;  // Taille de l'image en octets
    info_header.x_resolution_ppm = 0;           // Résolution horizontale en pixels par mètre
    info_header.y_resolution_ppm = 0;           // Résolution verticale en pixels par mètre
    info_header.num_colors = 0;                 // Nombre de couleurs dans la palette (0 si pas de palette)
    info_header.important_colors = 0;           // Nombre de couleurs importantes (0 pour toutes)

    char filename[100];
    // Si parameters->filename est "default", on génère un nom de fichier par défaut
    if(strcmp(parameters->filename,"default") == 0||strcmp(parameters->filename,"") == 0 ){
        // On récupère le mode de fractale (0 pour mandelbrot, 1 pour julia, 2 pour burningship
        char * modefract;
        if(parameters->modefract == 0)
            modefract = "mandelbrot";
        else if(parameters->modefract == 1)
            modefract = "julia";
        else if(parameters->modefract == 2)
            modefract = "burningship";
        else
            modefract = "unknown";

        // Le nom du fichier est de la forme : output/modefract_<annee>_<mois>_<jour>_<heure>_<minute>_<seconde>.bmp
        char filename[100];
        time_t now = time(NULL);
        struct tm * t = localtime(&now);
        sprintf(filename,"output/%s_%d_%d_%d_%d_%d_%d.bmp",modefract,t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
    }else{
        // Sinon, on utilise le nom de fichier donné par l'utilisateur
        sprintf(filename,"output/%s.bmp",parameters->filename);
    }

    // Pour s'assurer du cas ou l'utilisateur rentre un mauvais nom de fichier
    // Si le nom est invalide dans tous les cas on l'appelle default_output.bmp
    if(strcmp(filename,"") == 0){
        strcpy(filename,"output/default_output.bmp");
    }

    if(parameters->verbose)
        printf("[ ] \033[1mManager :\033[0m Ecriture du contenu de l'image\n");

    write_bmp(filename,&file_header,&info_header,pixels);

    if(parameters->verbose)
        printf("[ ] \033[1mManager :\033[0m Ecriture du fichier BMP terminée\n");
};