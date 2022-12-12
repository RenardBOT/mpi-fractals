//include std from c
#include <stdio.h>
#include <mpi.h>

// struct containing pixel as three int8_t
typedef struct rgb_t{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_t;

int main(int argc, char const *argv[])
{
    // dynamic array of 12 int from 0 to 360
    int * array = malloc(12*sizeof(int));
    for (int i = 0; i < 12; i++)
    {
        array[i] = i*30;
    }
    // copy array to an array of 4 rgb_t
    rgb_t * pixels = malloc(4*sizeof(rgb_t));
    for (int i = 0; i < 4; i++)
    {
        pixels[i].red = array[3*i];
        pixels[i].green = array[3*i+1];
        pixels[i].blue = array[3*i+2];
    }

    // print the array
    for (int i = 0; i < 4; i++)
    {
        printf("pixel %d : %d %d %d\n",i,pixels[i].red,pixels[i].green,pixels[i].blue);
    }

    return 0;   
}
//how to compile a file named sandbox.c ?
//gcc -o sandbox sandbox.c

void test_mpi(){
    //init MPI
    MPI_Init(NULL, NULL);
    //get the number of processes and the rank of the current process
    int world_size,world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // main MPI if statement
    if (world_rank == 0)
    {
        //master process
        printf("Hello from the master process %d of %d processes\n",world_rank,world_size);
    }else{
        //slave process
        printf("Hello from the slave process %d of %d processes\n",world_rank,world_size);
    }
}
