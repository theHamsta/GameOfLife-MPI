#include "board.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

board_t* board_create(unsigned int width, unsigned int height)
{
	board_t* board = malloc(sizeof(board_t));
	
	assert(board);
	
	board->height = height;
	board->height = width;
	
	assert ( width % BACTERIA_PER_FIELD == 0 && "width must be divisible by BACTERIA_PER_FIELD" );
	
	board->data = malloc( sizeof(field_t) * board->height * board->height / BACTERIA_PER_FIELD ); 
	
	if ( !board->data ) {
		printf("Allocation of board failed\n");
		exit(EXIT_FAILURE);
	}
	
	return board;
}

void board_destroy(board_t* board)
{
	free(board->data);
	free(board);
}

void board_reset(board_t* board)
{
	assert( board && board->data && "null pointer!" );
	
	memset(board->data, 0, sizeof(field_t) * board->height * board->height / BACTERIA_PER_FIELD );
}

void board_print(board_t* board)
{

}


