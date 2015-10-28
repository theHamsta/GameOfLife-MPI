#ifndef GLOBAL_BOARD_H
#define GLOBAL_BOARD_H

#include "board.h"

typedef struct global_board_s
{
	unsigned int global_width;
	unsigned int global_height;
	
	board_t* local_board;
	unsigned int local_x;
	unsigned int local_y;
	
	unsigned int mpi_rank;
	unsigned int mpi_sizeX;
	unsigned int mpi_sizeY;
	
	int mpi_communicator;
	
} global_board_t;

global_board_t* global_boardCreate( unsigned int width, unsigned int height );

void global_board_destroy( global_board_t* board );


#endif // GLOBAL_BOARD_H