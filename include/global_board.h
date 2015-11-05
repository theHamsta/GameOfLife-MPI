#ifndef GLOBAL_BOARD_H
#define GLOBAL_BOARD_H

#include "board.h"

#include <mpi.h>

#define BEGIN_MASTER_ONLY_SECTION(GLOBAL_BOARD) if ((GLOBAL_BOARD).mpi_rank == 0) {

#define END_MASTER_ONLY_SECTION }


#define GBOARD_NUM_DIRECTIONS (8)
#define GBOARD_NUM_DIAG_DIRECTIONS (4)
#define GBOARD_NUM_PERPENDICULAR_DIRECTIONS (4)

typedef struct globalBoard_s
{
	unsigned int global_width;
	unsigned int global_height;
	
	board_t* local_board;
	unsigned int local_x;
	unsigned int local_y;
	
	unsigned int mpi_rank;
	unsigned int mpi_rankX;
	unsigned int mpi_rankY;
	unsigned int mpi_sizeX;
	unsigned int mpi_sizeY;
	
	
	int neighbourLeft;
	int neighbourRight;
	int neighbourUp;
	int neighbourDown;
	
	// diagonal neighbours
	int neighbourTL;
	int neighbourTR;
	int neighbourBL;
	int neighbourBR;
	
	
	MPI_Comm mpi_comm;
	
	field_t* sendBufLeft;
	field_t* sendBufRight;
	field_t* sendBufUp;
	field_t* sendBufDown;

	field_t* recvBufLeft;
	field_t* recvBufRight;
	field_t* recvBufUp;
	field_t* recvBufDown;
	field_t* recvBufCourners;
	
	MPI_Request* reqSend;
	MPI_Request* reqRecv;
	
} globalBoard_t;

globalBoard_t* globalBoard_create( unsigned int width, unsigned int height, int rank, int numRanksX, int numRanksY );

void globalBoard_print( globalBoard_t* board );

void globalBoard_printDebug( globalBoard_t* board );

void globalBoard_fillRandomly ( globalBoard_t* board );

void globalBoard_step ( globalBoard_t* board );

void globalBoard_destroy( globalBoard_t* board );

board_t* globalBoard_uniteLocalBoards( globalBoard_t* board );


#endif // GLOBAL_BOARD_H