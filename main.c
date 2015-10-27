// #include <mpi.h>
#include <stdio.h>

#include "board.h"


#define GLOBAL_BOARD_WIDTH 12
#define GLOBAL_BOARD_HEIGHT 3
#define PERIODIC_BOUNDARY_CONDITIONS true



int main(int argc, char** argv) {
//     // Initialize the MPI environment
//     MPI_Init(NULL, NULL);
// 
//     // Get the number of processes
//     int world_size;
//     MPI_Comm_size(MPI_COMM_WORLD, &world_size);
// 
//     // Get the rank of the process
//     int world_rank;
//     MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
// 
//     // Get the name of the processor
//     char processor_name[MPI_MAX_PROCESSOR_NAME];
//     int name_len;
//     MPI_Get_processor_name(processor_name, &name_len);
// 
//     // Print off a hello world message
//     printf("Hello world from processor %s, rank %d"
//            " out of %d processors\n",
//            processor_name, world_rank, world_size);
// 
// 	
// 	
// 	
//     // Finalize the MPI environment.
//     MPI_Finalize();
	
	
	board_t* board = board_create(GLOBAL_BOARD_WIDTH, GLOBAL_BOARD_HEIGHT);
	board_fillRandomly(board);
	board_print(board);
	
	
	
	printf("%x\n",FIELD_ALL_ELEMENTS_MASK);
	
}
