#include "board.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void board_init(board_t* board, unsigned int width, unsigned int height)
{
	board->m_height = height;
	board->m_width = width;
	
	assert ( width % BACTERIA_PER_FIELD == 0 && "width must be divisible by BACTERIA_PER_FIELD" );
	
	board->m_data = malloc( sizeof(field_t) * board->m_height * board->m_width / BACTERIA_PER_FIELD ); 
	
	if ( !board->m_data ) {
		printf("malloc failed\n");
		exit(EXIT_FAILURE);
	}
}

void board_destroy(board_t* board)
{
	free(board->m_data);
	free(board);
}

void board_reset(board_t* board)
{
	assert( board && board->m_data && "null pointer!" );
	
	memset(board->m_data, 0, sizeof(field_t) * board->m_height * board->m_width / BACTERIA_PER_FIELD );
}

void board_print(board_t* board)
{

}


