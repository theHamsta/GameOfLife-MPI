#include <mpi.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>




#define xstr(s) str(s)
#define str(s) #s

#include "board.h"
#include "global_board.h"


#define MAGIC_NUMBER 12



void clearScreen() {
	printf("\e[1;1H\e[2J");
}


int main(int argc, char** argv) {
#ifdef NDEBUG
	printf("Warning NDEBUG defined! Debug mode?\n");
	fflush(stdout);
#endif
	
	int numRounds;
	int globalBoardWidth;
	int globalBoardHeight;
	int numMpiRankX;
	int numMpiRankY;
	bool bPrintOutput;
	
	double start, end, max_end = 0, min_start = 10000;
	
	// Initialize the MPI environment
    MPI_Init(NULL, NULL);
	// Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	
	int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	
	if( argc != 7 ) {
		if(world_rank == 0) {
			printf("%i\n", argc);
			printf("Usage:\n"
				"\t gameoflife 	numRounds globalBoardWidth(in multiples of 4) globalBoardHeight(in multiples of 3) numMpiRankX numMpiRankY bPrintOutput[1 for output]"
			);
		}
		MPI_Finalize();
		exit( EXIT_SUCCESS );	
	} else {
		int numVars = 0;
		// some how first call of sscanf always reads 0. wtf?
		sscanf(argv[5],"%i" , &numMpiRankY);
		// these calls should return the right results
		numVars += sscanf(argv[1],"%i" , &numRounds);
		numVars += sscanf(argv[2],"%i" , &globalBoardWidth);
		numVars += sscanf(argv[3],"%i" , &globalBoardHeight);
		numVars += sscanf(argv[4],"%i" , &numMpiRankX);
		numVars += sscanf(argv[5],"%i" , &numMpiRankY);
		numVars += sscanf(argv[6],"%i" , &bPrintOutput);

		
		if(numVars != 6){
			if( world_rank == 0 ) {
				printf("Error parsing arguments!\n");
			}
			MPI_Finalize();
			exit( EXIT_SUCCESS );	
		}
		
	}
	
	if( world_size != numMpiRankX * numMpiRankY ) {
		if( world_rank == 0 ) {
			printf("Please start with %i processes! Started with %i\n", numMpiRankX * numMpiRankY, world_size);
		}
		MPI_Finalize();
		exit( EXIT_SUCCESS );	
	}
	
	

	if(world_rank == 0){
		
		printf("Started with the following arguments:\n"
			"numRounds: %i\n"
			"globalBoardWidth: %i*" xstr(BACTERIA_PER_FIELD_X) "\n"
			"globalBoardHeight: %i*" xstr(BACTERIA_PER_FIELD_Y) "\n"
			"numMpiRankX: %i\n"
			"numMpiRankY: %i\n"
			"bPrintOutput: %i\n",
		 	 numRounds,
	 globalBoardWidth,
	 globalBoardHeight,
	 numMpiRankX,
	 numMpiRankY,
	 bPrintOutput
		);
	}



	field_initLuts();

	if ( numMpiRankX == 1 && numMpiRankY == 1 ) {
		board_t* board = board_create(globalBoardWidth, globalBoardHeight);

		board_fillRandomly(board);
		
		clock_t start = clock();
		for( int i = 0; i < NUM_ROUNDS; i++ ) {
			board_step(board);
			if(bPrintOutput) {
				clearScreen();
				board_print(board);
				usleep(100000);  		
			}
		}
		clock_t end = clock();
		float seconds = (float)(end - start) / CLOCKS_PER_SEC;
		
		printf("\n%fs\n", seconds);
		
	} else {


		srand(MAGIC_NUMBER + world_rank);
		
		globalBoard_t* gBoard = globalBoard_create(globalBoardWidth, globalBoardHeight,  world_rank, numMpiRankX, numMpiRankY);
		
		globalBoard_fillRandomly(gBoard);
		

		MPI_Barrier(gBoard->mpi_comm);
		start = MPI_Wtime();
		
		for ( int i = 0; i < numRounds; i++ ) {

			globalBoard_step(gBoard);
			
			if(bPrintOutput) {
				clearScreen();
				globalBoard_print(gBoard);
				usleep(100000);  		
			}
		}
		MPI_Barrier(gBoard->mpi_comm);
		end = MPI_Wtime();
		
		globalBoard_destroy(gBoard);
		
		if ( world_rank == 0 )
			printf("\n%fs\n", end-start);
	}
	
// 	field_initLuts();
// // 	
// 	board_t* board = board_create(GLOBAL_BOARD_WIDTH, GLOBAL_BOARD_HEIGHT);
// // 
//  	board_fillRandomly(board);
// 	
// 	board_print(board);
// 	for( int i = 0; i < numRounds; i++ ) {
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
