#include <mpi.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "board.h"
#include "global_board.h"


#define GLOBAL_BOARD_WIDTH 4*9
#define GLOBAL_BOARD_HEIGHT 3*9
#define NUM_ROUNDS 100
#define GRAPHICAL_OUTPUT true

#define NUM_MPI_RANKS_X 9
#define NUM_MPI_RANKS_Y 9

#define MAGIC_NUMBER 12

void clearScreen() {
	printf("\e[1;1H\e[2J");
}

void waitFor (time_t secs) {
    time_t retTime = clock() + secs;     // Get finishing time.
    while (time(0) < retTime);    // Loop until it arrives.
}

int main(int argc, char** argv) {
#ifdef NDEBUG
	printf("Warning! Debug mode\n");
	fflush(stdout);
#endif
	
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);



    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	
	
	field_initLuts();


	srand(MAGIC_NUMBER + world_rank);
	
	globalBoard_t* gBoard = globalBoard_create(GLOBAL_BOARD_WIDTH, GLOBAL_BOARD_HEIGHT,  world_rank, NUM_MPI_RANKS_X, NUM_MPI_RANKS_Y);
	
	globalBoard_fillRandomly(gBoard);
	

	

	for ( int i = 0; i < NUM_ROUNDS; i++ ) {

		globalBoard_step(gBoard);
		if(GRAPHICAL_OUTPUT) {
			clearScreen();
			globalBoard_print(gBoard);
			usleep(100000);  		
		}
	}
	globalBoard_destroy(gBoard);
	
	
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
	

    MPI_Finalize();
	
	
	
	
	
}
