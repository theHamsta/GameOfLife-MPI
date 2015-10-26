#include "board.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// from: http://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer
uint32_t popcount(uint32_t i)
{
     i = i - ((i >> 1) & 0x55555555);
     i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
     return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}


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
			printf("%c", board_value_at(board, x, y) ? 'X' : "_");
		}
		printf("\n");
	}
}

bool board_value_at(board_t* board, unsigned int x, unsigned int y)
{
	field_t field = board->data[ (y * board->width + x) / BACTERIA_PER_FIELD ];
	return (field.oldCells >> (BACTERIA_PER_FIELD - 1 - (y % BACTERIA_PER_FIELD)) & 1;
}


void field_update( field_t* field ) {
	for ( int i = 0; i < BACTERIA_PER_FIELD; i++ ) {
		
		uint32_t neighbours = (*field >> i) & FIELD_NEIGHBOUR_MASK;
		uint32_t numNeighbours = popcount(neighbours);
		
		bool bWasAlive = (field->oldCells >> i) & 1;
		
		bool bIsAlive = false;
		if( numNeighbours == 3 ) {
			bIsAlive = true;
		} else if ( bWasAlive && numNeighbours == 2 ) {
			bIsAlive = true;
		}
		*field.newCells = bIsAlive << i;
	}
}


void board_step( board_t* board )
{
	field_t* above = 0;
	field_t* current = board->data;
	field_t* below = board->data + board->width / BACTERIA_PER_FIELD;
	
	assert( board->height > 1 && "Board to small to process");
	
	for ( unsigned int y = 0; y < board->height; y++ ) {
		for( unsigned int x = 0; x < board->width / BACTERIA_PER_FIELD; x++ ) {
			
			field_update(current);
			current++;
		}

	}
	for ( unsigned int y = 0; y < board->height; y++ ) {
		for( unsigned int x = 0; x < board->width / BACTERIA_PER_FIELD; x++ ) {
			
			field_update(current);
			
			
			
			above++;
			current++;
			below++;
		}
		if ( y == 0 ) {
			above = board->data;
		} 
	}
}


