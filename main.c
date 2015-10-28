// #include <mpi.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "board.h"



#define GLOBAL_BOARD_WIDTH 4*2
#define GLOBAL_BOARD_HEIGHT 3*1
#define NUM_ROUNDS 2
#define PERIODIC_BOUNDARY_CONDITIONS true


void clearScreen() {
	printf("\e[1;1H\e[2J");
}

void waitFor (unsigned int secs) {
    int retTime = time(0) + secs;     // Get finishing time.
    while (time(0) < retTime);    // Loop until it arrives.
}

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
	
// 	field_t a, b;
// 	
// 	a.val = 0;
// 	b.val = FIELD_ABOVE_MASK | FIELD_BELOW_MASK;
// // 	field_broadcastTopLeft(&a,&b);
// 	
// 	field_printDebugAllLines(&a);
// 	field_printDebugAllLines(&b);
	



#ifdef DEBUG
	printf("Warning! Debug mode");
#endif

	board_t* board = board_create(GLOBAL_BOARD_WIDTH, GLOBAL_BOARD_HEIGHT);

 	board_fillRandomly(board);
	
	board_print(board);
	for( int i = 0; i < NUM_ROUNDS; i++ ) {
		waitFor(1);
		
		board_step(board);
		
// 		clearScreen();
		board_printDebug(board);
		fflush(stdout); 
	}
	
	
	
	
	
}
