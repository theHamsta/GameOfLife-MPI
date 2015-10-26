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
	
} global_board_t;

global_board_t* global_board_create( unsigned int width, unsigned int height );

void global_board_destroy( global_board_t* board );


#endif // GLOBAL_BOARD_H