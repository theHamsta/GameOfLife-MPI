#include <mpi.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define CHECK_RESULTS

#define xstr(s) str(s)
#define str(s) #s

#include "board.h"
#include "global_board.h"


#define MAGIC_NUMBER 12

#ifdef CHECK_RESULTS_DURING_RUN
#define CHECK_RESULTS
#endif


void clearScreen() {
	printf("\e[1;1H\e[2J");
}


int main(int argc, char** argv) {

	
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
	

	if (world_rank == 1) {
#ifndef NDEBUG
		printf("Warning: NDEBUG not defined! Debug mode?\n");
		fflush(stdout);
	
#endif
#ifdef CHECK_RESULTS
		printf("CHECK_RESULTS defined!\n");
		fflush(stdout);
	
#endif
#ifdef CHECK_RESULTS_DURING_RUN
		printf("CHECK_RESULTS_DURING_RUN defined!\n");
		fflush(stdout);
	
#endif
	}

	
	
	if( argc != 7 ) {
		if(world_rank == 0) {
			printf("Usage:\n"
				"\t gameoflife 	numRounds globalBoardWidth(in multiples of "xstr(BACTERIA_PER_FIELD_X)") globalBoardHeight(in multiples of " xstr(BACTERIA_PER_FIELD_Y) ") numMpiRankX numMpiRankY bPrintOutput[1 for output]"
			);
		}
		MPI_Finalize();
		exit( EXIT_SUCCESS );	
	} else {
		int numVars = 0;
		// some how first call of sscanf always reads 0. wtf?
// 		sscanf(argv[5],"%i" , &numRounds);
		
		// these calls should return the right results
			numVars += sscanf(argv[2],"%i" , &numRounds);
		
		numVars += sscanf(argv[2],"%i" , &globalBoardWidth);
		numVars += sscanf(argv[3],"%i" , &globalBoardHeight);
		numVars += sscanf(argv[4],"%i" , &numMpiRankX);
		numVars += sscanf(argv[5],"%i" , &numMpiRankY);
		numVars += sscanf(argv[6],"%i" , &bPrintOutput);
		numVars += sscanf(argv[1],"%i" , &numRounds);
		
		if(numVars != 7){
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
		for( int i = 0; i < numRounds; i++ ) {
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
		
	} else if ( numMpiRankX == 1 && numMpiRankY == 1 ) {
		if(world_rank == 0) {

			printf("This programme version cannot handle 1d MPI Cartesian coordinates. Both numMpiRankX and numMpiRankY must be greater than 1 or both equal to 1.\n"
			);
		}
		MPI_Finalize();
		exit( EXIT_SUCCESS );
	} else {


		srand(MAGIC_NUMBER + world_rank);
		
		globalBoard_t* gBoard = globalBoard_create(globalBoardWidth, globalBoardHeight,  world_rank, numMpiRankX, numMpiRankY);
		
		globalBoard_fillRandomly(gBoard);
#if  defined(CHECK_RESULTS) || defined(CHECK_RESULTS_DURING_RUN)
		
		board_t* checkResultsBoard = globalBoard_uniteLocalBoards(gBoard);

#endif		

		MPI_Barrier(gBoard->mpi_comm);
		
		start = MPI_Wtime();
		
		for ( int i = 0; i < numRounds; i++ ) {

			globalBoard_step(gBoard);
#ifdef CHECK_RESULTS_DURING_RUN
			board_step(checkResultsBoard);
#endif
			if(bPrintOutput) {
				clearScreen();
				globalBoard_print(gBoard);
#ifdef CHECK_RESULTS_DURING_RUN
				if( world_rank == 0 ) {
					board_print(checkResultsBoard);
				}
#endif 
				usleep(100000);  	
			}
			
#ifdef CHECK_RESULTS_DURING_RUN
				

				board_t* calculatedBoard = globalBoard_uniteLocalBoards(gBoard);
				if( world_rank == 0 ) {

					for (int y = 0; y < calculatedBoard->heightDiv3; y++ ) {
						for ( int x = 0; x < calculatedBoard->widthDiv4; x++ ) {
							if ( (BOARD_GET_FIELD_PTR(calculatedBoard, x,y)->val ) != (BOARD_GET_FIELD_PTR(checkResultsBoard, x,y)->val )) {
								board_printDebug(calculatedBoard);
								board_printDebug(checkResultsBoard);
								printf("%i, (%i,%i)\n", i, x, y);
								field_printDebugAllLines(BOARD_GET_FIELD_PTR(calculatedBoard, x,y));
								field_printDebugAllLines(BOARD_GET_FIELD_PTR(checkResultsBoard, x,y));
								exit( EXIT_SUCCESS );
							}
						}
					}
				}
#endif
		}
		MPI_Barrier(gBoard->mpi_comm);

		end = MPI_Wtime();
#ifdef CHECK_RESULTS
		board_t* calculatedBoard = globalBoard_uniteLocalBoards(gBoard);
		if(world_rank == 1){
			for (int i = 0; i < numRounds; i++ ) {
				board_step(checkResultsBoard);
			}
			for (int y = 0; y < calculatedBoard->heightDiv3; y++ ) {
				for ( int x = 0; x < calculatedBoard->widthDiv4; x++ ) {
					if ( (BOARD_GET_FIELD_PTR(calculatedBoard, x,y)->val ) != (BOARD_GET_FIELD_PTR(checkResultsBoard, x,y)->val )) {
						board_printDebug(calculatedBoard);
						board_printDebug(checkResultsBoard);
						printf("error at (%i,%i)\n", x, y);
						field_printDebugAllLines(BOARD_GET_FIELD_PTR(calculatedBoard, x,y));
						field_printDebugAllLines(BOARD_GET_FIELD_PTR(checkResultsBoard, x,y));
						exit( EXIT_SUCCESS );
					}
				}
			}
			
		}
#endif

		
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
