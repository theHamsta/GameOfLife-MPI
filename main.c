#include <mpi.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "board.h"
#include "global_board.h"


#define GLOBAL_BOARD_WIDTH 4*2
#define GLOBAL_BOARD_HEIGHT 3*2
#define NUM_ROUNDS 100
#define PERIODIC_BOUNDARY_CONDITIONS true


void clearScreen() {
	printf("\e[1;1H\e[2J");
}

void waitFor (time_t secs) {
    time_t retTime = clock() + secs;     // Get finishing time.
    while (time(0) < retTime);    // Loop until it arrives.
}

int main(int argc, char** argv) {
	printf("Warning! Debug mode\n");
	fflush(stdout);
	
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	
	
	field_initLuts();

	
// 	srand(time(NULL) + world_rank);
	srand(12 + world_rank);
	globalBoard_t* gBoard = globalBoard_create(GLOBAL_BOARD_WIDTH, GLOBAL_BOARD_HEIGHT, world_rank, 2, 2);
	fflush(stdout);
	
	
	globalBoard_fillRandomly(gBoard);
	
	
	globalBoard_print(gBoard);
	
	for ( int i = 0; i < NUM_ROUNDS; i++ ) {
		globalBoard_step(gBoard);
		
	}

	

// #ifdef NDEBUG
// 	printf("Warning! Debug mode\n");
// #endif

	
	
// 	field_initLuts();
// // 	
// 	board_t* board = board_create(GLOBAL_BOARD_WIDTH, GLOBAL_BOARD_HEIGHT);
// // 
//  	board_fillRandomly(board);
// 	
// 	board_print(board);
// 	for( int i = 0; ; i++ ) {
// 		usleep(100000);  
// 		
// 		board_broadcastNeighbourhoods(board);
// 		board_updateFields(board);
// 		clearScreen();
// 		board_print(board);
// 		fflush(stdout); 
// 	}

// 	clock_t start = clock();
// 	for( int i = 0; i < NUM_ROUNDS; i++ ) {
// 		board_step(board);
// // 		printf("%i\n", i);
// // 		fflush(stdout); 
// 	}
// 	clock_t end = clock();
// 	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
// 	
// 	printf("%fs\n", seconds);
// 	fflush(stdout); 
	
		
    // Finalize the MPI environment.
    MPI_Finalize();
	
	
	
	
	
}
