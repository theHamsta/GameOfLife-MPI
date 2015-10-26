#include "global_board.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>


global_board_t* global_board_create(unsigned int width, unsigned int height)
{
	global_board_t* global_board = malloc(sizeof(global_board_t));
	
	assert(global_board);
	
	assert(false && "not implemented");
	
}


void global_board_destroy(global_board_t* board)
{
	board_destroy(board->local_board);
	free(board);
}
