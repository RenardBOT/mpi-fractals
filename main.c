// Déclaration des entêtes

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parameters.h"
#include "save_bmp.h"
#include "tasks.h"


// Signatures des fonctions

int main(int argc, char **argv);
void print_parameters(parameters_t * parameters);
void failure(char * error, int world_rank);
void warning(char * warning, int world_rank);
void write_file(parameters_t * parameters, bitmap_rgb * pixels);


// Déclaration des fonctions

int main(int argc, char** argv)
{
    parameters_t * parameters   = (parameters_t *)malloc(sizeof(*parameters));
    parameters_calc_t * parameters_calc = (parameters_calc_t *)malloc(sizeof(*parameters_calc));
    if(parameters == NULL){
        printf("Erreur : Echec de l'allocation de mémoire pour les paramètres\n");
        exit(1);
    }
    if(parameters_calc == NULL){
        printf("Erreur : Echec de l'allocation de mémoire pour les paramètres de calcul\n");
        exit(1);
    }

    
    MPI_Init(NULL, NULL);

    int world_size,world_rank,name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];        
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);         // Nombre de tâches
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);         // Rang de la tâche
    MPI_Get_processor_name(processor_name, &name_len);  // Nom et taille du nom du noeud

    init_parameters(parameters);
    parse_args(argc,argv,parameters,world_rank,world_size);
    init_parameters_calc(parameters,parameters_calc);

    

    

    if(world_rank == 0){
        // declare pixels
        bitmap_rgb * pixels = (bitmap_rgb *)malloc(parameters->width*parameters->height*sizeof(bitmap_rgb));
        print_parameters(parameters);
        print_parameters_calc(parameters_calc);

        manager_task(parameters,MPI_COMM_WORLD,pixels);
        write_file(parameters,pixels);
        //test_picture(parameters);
        free(pixels);
        
    }
    else{
        worker_task(parameters,parameters_calc,MPI_COMM_WORLD);
    }

    // Finalize the MPI environment.
    MPI_Finalize();

    free(parameters);
    free(parameters_calc);
    return 0;

    
}

// 8 - Déclaration des fonctions 

// Affiche un message d'erreur en rouge si le rang de la tâche est 0
void failure(char * error, int world_rank){
    if(world_rank == 0)
        printf("\033[91;1mErreur : \033[0m%s\n\nSortie forcée du programme\n",error);
    exit(EXIT_FAILURE);
};

// Affiche un message d'avertissement en jaune si le rang de la tâche est 0
void warning(char * warning, int world_rank){
    if(world_rank == 0)
        printf("\033[93;1mAttention : \033[0m%s\n",warning); 
};

void test_picture(parameters_t * parameters){
    // Initialise un tableau de bitmap_rgb de taille width*height et rempli d'un damier de pixels rouges et verts
    bitmap_rgb * pixels = (bitmap_rgb *)malloc(parameters->width*parameters->height*sizeof(bitmap_rgb));
    for(int i=0;i<parameters->height;i++){
        for(int j=0;j<parameters->width;j++){
            if((i+j)%2 == 0){
                pixels[i*parameters->width+j].red = 0;
                pixels[i*parameters->width+j].green = 0;
                pixels[i*parameters->width+j].blue = 0;
            }
            else{ 
                pixels[i*parameters->width+j].red = 255;
                pixels[i*parameters->width+j].green = 255;
                pixels[i*parameters->width+j].blue = 255;
            }
        }
    }
    write_file(parameters,pixels);
    free(pixels);
};

void write_file(parameters_t * parameters, bitmap_rgb * pixels){

    bitmap_file_header file_header;
    bitmap_info_header info_header;

    // La taille d'une ligne doit être un multiple de 4
    int line_size_bytes = parameters->width*sizeof(bitmap_rgb);
    if(line_size_bytes%4 != 0)
        line_size_bytes += 4 - line_size_bytes%4;

    file_header.type = 0x4D42;
    file_header.size = sizeof(file_header) + sizeof(info_header) + line_size_bytes*parameters->height;
    file_header.reserved = 0;
    file_header.offset = sizeof(file_header) + sizeof(info_header);

    info_header.size = sizeof(info_header);
    info_header.width_px = parameters->width;
    info_header.height_px = parameters->height;
    info_header.planes = 1;
    info_header.bits_per_pixel = sizeof(bitmap_rgb)*8;
    info_header.compression = 0;
    info_header.image_size_bytes = parameters->height*line_size_bytes;
    info_header.x_resolution_ppm = 0;  
    info_header.y_resolution_ppm = 0;
    info_header.num_colors = 0;
    info_header.important_colors = 0;

    write_bmp("output.bmp",&file_header,&info_header,pixels);
};