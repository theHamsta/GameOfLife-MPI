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
	for ( unsigned int y = 0; y < board->height; y++ ) {
		for ( unsigned int x = 0; x < board->width; x++ ) {
			printf("%c", board_value_at(board, x, y) ? 'X' : '_');
		}
		printf("\n");
	}
}

bool board_value_at(board_t* board, unsigned int x, unsigned int y)
{
	field_t field = board->data[ (y * board->width + x) / BACTERIA_PER_FIELD ];
	return (field.bitfield.oldCells >> (BACTERIA_PER_FIELD - 1 - (y % BACTERIA_PER_FIELD))) & 1;
}




void board_step( board_t* board )
{
	field_t* above = 0;
	field_t* current = board->data;
	field_t* below = board->data + board->width / BACTERIA_PER_FIELD;
	
	assert( board->height > 1 && "Board to small to process");
	
	for ( unsigned int y = 0; y < board->height; y++ ) {
		for( unsigned int x = 0; x < board->width / BACTERIA_PER_FIELD; x++ ) {
			
			if(FIELD_IS_ALIVE(*current)) {
				field_update(current);
			}
			current++;
		}

	}
	for ( unsigned int y = 0; y < board->height; y++ ) {
		for( unsigned int x = 0; x < board->width / BACTERIA_PER_FIELD; x++ ) {
			
			
			if(field_has_changed(current)) {
				BROADCAST_LEFT			(*current, *(current - 1));
				if ( y != 0 ) {
					if ( x != 0 )
						BROADCAST_TOP_LEFT(*current, *(above - 1));
					BROADCAST_TOP			(*current, *(above));
					if ( x != board->width )
						BROADCAST_TOP_RIGHT	(*current, *(above + 1));
				}
				BROADCAST_RIGHT			(*current, *(current + 1));
				BROADCAST_BOTTOM_RIGHT	(*current, *(below + 1));
				BROADCAST_BOTTOM		(*current, *(below));
				BROADCAST_BOTTOM_LEFT	(*current, *(below - 1));
			}
			
			
			
			above++;
			current++;
			below++;
		}
		if ( y == 0 ) {
			above = board->data;
		} 
	}
}


