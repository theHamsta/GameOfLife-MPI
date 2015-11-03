#include <mpi.h>
#include <stdio.h>
#include <stdint.h>
#include<stdlib.h>
#include <time.h>
#include <unistd.h>

#include "board.h"



#define GLOBAL_BOARD_WIDTH 4*20
#define GLOBAL_BOARD_HEIGHT 3*10
#define NUM_ROUNDS 200
#define PERIODIC_BOUNDARY_CONDITIONS true


void clearScreen() {
	printf("\e[1;1H\e[2J");
}

void waitFor (time_t secs) {
    time_t retTime = clock() + secs;     // Get finishing time.
    while (time(0) < retTime);    // Loop until it arrives.
}

int main(int argc, char** argv) {
    // Initialize the MPI environment
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
// 	int coords[2] = { 1, 2 };
// 	int communitcator = MPI_Cart_coords(MPI_COMM_WORLD, world_rank, 2, coords);



#ifdef NDEBUG
	printf("Warning! Debug mode\n");
#endif

	field_initLuts();
	
	board_t* board = board_create(GLOBAL_BOARD_WIDTH, GLOBAL_BOARD_HEIGHT);

 	board_fillRandomly(board);
	
	board_print(board);
	for( int i = 0; ; i++ ) {
		usleep(100000);  
		
		board_step(board);
		
		clearScreen();
		board_print(board);
		fflush(stdout); 
	}
/*	
	clock_t start = clock();
	for( int i = 0; i < NUM_ROUNDS; i++ ) {
		board_step(board);
	}
	clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	
	printf("%fs\n", seconds);
	fflush(stdout); 
	*/
		
    // Finalize the MPI environment.
//     MPI_Finalize();
	
	
	
	
	
}
