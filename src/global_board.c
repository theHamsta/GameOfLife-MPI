#include "global_board.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>




// typedef struct globalBoard_s
// {
// 	unsigned int global_width;
// 	unsigned int global_height;
// 	
// 	board_t* local_board;
// 	unsigned int local_x;
// 	unsigned int local_y;
// 	
// 	unsigned int mpi_rankX;
// 	unsigned int mpi_rankY;
// 	unsigned int mpi_sizeX;
// 	unsigned int mpi_sizeY;
// 	
// 	unsigned int neighbourLeft;
// 	unsigned int neighbourRight;
// 	unsigned int neighbourUp;
// 	unsigned int neighbourDown;
// 	
// 	int mpi_comm;
// 	
// } globalBoard_t;

#define BEGIN_MASTER_ONLY_SECTION(GLOBAL_BOARD) if (((GLOBAL_BOARD).mpi_rankX == 0) && ((GLOBAL_BOARD).mpi_rankY == 0)) {

#define END_MASTER_ONLY_SECTION }




globalBoard_t* globalBoard_create(unsigned int width, unsigned int height, int rank, int numRanksX, int numRanksY )
{
	assert( rank < numRanksX * numRanksY );
	
	globalBoard_t* global_board = malloc(sizeof(globalBoard_t));
	
	int dims[2];
	dims[0] = numRanksX;
	dims[1] = numRanksY;
	int periodic[2] = { true, true };
	int reorder = true;
	int coords[2];
	
	MPI_Comm comm;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodic , reorder, &comm);
	MPI_Cart_coords(comm, rank, 2, coords);

	// Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
	
	MPI_Barrier(MPI_COMM_WORLD);
	printf("Hello world from processor %s, rank(%i,%i) in Cartesian coordinates\n", processor_name, coords[0], coords[1]);
	fflush(stdout);
	MPI_Barrier(MPI_COMM_WORLD);
	
	global_board->global_width = width;
	global_board->global_height = height;
	
	assert ( numRanksX * (width / numRanksX) + (width % numRanksX) == width );
	
	int localWidth = (coords[0] == numRanksX) ? (width / numRanksX) : (width / numRanksX) + (width % numRanksX);
	int localHeight = (coords[1] == numRanksY) ? (height / numRanksY) : (height / numRanksY) + (height % numRanksY);
	
	global_board->local_board = board_create(localWidth, localHeight);
	global_board->local_x = coords[0] * (width / numRanksX);
	global_board->local_y = coords[1] * (height / numRanksY);
	
	global_board->mpi_rankX = coords[0];
	global_board->mpi_rankY = coords[1];
	global_board->mpi_sizeX = numRanksX;
	global_board->mpi_sizeY = numRanksY;
	
	
	
	global_board->neighbourLeft = (coords[0] + numRanksX - 1) % numRanksX;
	global_board->neighbourRight = (coords[0] + numRanksX + 1) % numRanksX;
	global_board->neighbourDown = (coords[1] + numRanksY - 1) % numRanksY;
	global_board->neighbourUp = (coords[1] + numRanksY + 1) % numRanksY;
	
	global_board->mpi_comm = comm;
	
	assert(global_board);

	
}

void globalBoard_fillRandomly(globalBoard_t* board)
{
	MPI_Barrier(board->mpi_comm);
	for ( int y = 0; y < board->mpi_sizeY; y++ ) {
		for ( int x = 0; x < board->mpi_sizeX; x++ ) {
			if ( board->mpi_rankX == x && board->mpi_rankY == y ) {
				board_fillRandomly(board->local_board);
			}
			MPI_Barrier(board->mpi_comm);
		}
	}
}


void globalBoard_print(globalBoard_t* board)
{
	
	MPI_Barrier(board->mpi_comm);
	
	
	for ( int y = 0; y < board->mpi_sizeY; y++ ) {
		for ( int x = 0; x < board->mpi_sizeX; x++ ) {
			if ( board->mpi_rankX == x && board->mpi_rankY == y ) {
				
				board_print(board->local_board);
				fflush(stdout);
// 				usleep(1000000);
			}
			MPI_Barrier(MPI_COMM_WORLD);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		BEGIN_MASTER_ONLY_SECTION(*board)
			printf("===================\n");
			fflush(stdout);
		END_MASTER_ONLY_SECTION
	}
}




void globalBoard_destroy(globalBoard_t* board)
{
	board_destroy(board->local_board);
	free(board);
}


