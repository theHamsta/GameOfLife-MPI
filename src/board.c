#include "board.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>





board_t* board_create(unsigned int width, unsigned int height)
{
	board_t* board = malloc(sizeof(board_t));
	
	assert(board);
	
	board->height = height;
	board->width = width;
	
	assert ( width % BACTERIA_PER_FIELD_X == 0 && "width must be divisible by BACTERIA_PER_FIELD_X" );
	assert ( height % BACTERIA_PER_FIELD_Y == 0 && "height must be divisible by BACTERIA_PER_FIELD_Y" );
	
	int fieldsForBoard = board->height * board->height / BACTERIA_PER_FIELD;
	int fieldsPadding = 2 * BOARD_PADDING_Y * (board->width + 2 * BOARD_PADDING_X) + // padding row above and below field
						2 * BOARD_PADDING_X * board->height; 					   // padding column left and right of field
	board->data = malloc( sizeof(field_t) * fieldsForBoard * fieldsPadding ); 
	
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
	field_t* current = BOARD_PTR_FIRST_FIELD(*board);
	for ( unsigned int y = 0; y < board->height / BACTERIA_PER_FIELD_Y; y++ ) {
		for ( unsigned int l = 0; l < BACTERIA_PER_FIELD_Y; l++ ) {
			for ( unsigned int x = 0; x < board->width; x++ ) {

				field_print(current, l);
				current++;
			}
			current -= board->width;
			printf("\n");
		}
		current += 2 * BOARD_PADDING_X;

	}
}

/*
bool board_value_at(board_t* board, unsigned int x, unsigned int y)
{
	field_t field = board->data[ (y * board->width + x) / BACTERIA_PER_FIELD ];
	return (field.bitfield.oldCells >> (BACTERIA_PER_FIELD - 1 - (y % BACTERIA_PER_FIELD))) & 1;
}*/




void board_step( board_t* board )
{
	field_t* above = board->data + BOARD_PADDING_X;
	field_t* current = BOARD_PTR_FIRST_FIELD(*board);
	field_t* below = current + BOARD_LINE_SKIP(*board);
	
	assert( board->height > 1 && "Board to small to process");
	
	for ( unsigned int y = 0; y < board->height; y++ ) {
		for( unsigned int x = 0; x < board->width / BACTERIA_PER_FIELD; x++ ) {
			
			if(current->val != 0) {
				field_update(current);
			}
			current++;
		}
		current += 2 * BOARD_PADDING_X;
	}
	
	current = BOARD_PTR_FIRST_FIELD(*board);
	
	for ( unsigned int y = 0; y < board->height; y++ ) {
		for( unsigned int x = 0; x < board->width / BACTERIA_PER_FIELD; x++ ) {
			
			
			if(FIELD_WAS_CHANGED(*current)) {
// 				BROADCAST_LEFT			(*current, *(current - 1));
// 				BROADCAST_TOP_LEFT		(*current, *(above - 1));
// 				BROADCAST_TOP			(*current, *(above));
// 				BROADCAST_TOP_RIGHT		(*current, *(above + 1));
// 				BROADCAST_RIGHT			(*current, *(current + 1));
// 				BROADCAST_BOTTOM_RIGHT	(*current, *(below + 1));
// 				BROADCAST_BOTTOM		(*current, *(below));
// 				BROADCAST_BOTTOM_LEFT	(*current, *(below - 1));
				
				FIELD_UNSET_WAS_CHANGED(*current);
			}
			
			
			
			above++;
			current++;
			below++;
		}
		above += 2 * BOARD_PADDING_X;
		current += 2 * BOARD_PADDING_X;
		below += 2 * BOARD_PADDING_X;
	}
}

void board_fillRandomly(board_t* board)
{
	srand(time(NULL));
	field_t* current = BOARD_PTR_FIRST_FIELD(*board);
	for ( unsigned int y = 0; y < board->height; y++ ) {
		for ( unsigned int x = 0; x < board->width; x++ ) {
			
			current->val = rand() & FIELD_ALL_ELEMENTS_MASK;
			current++;
		}
		current += 2 * BOARD_PADDING_X;
		printf("\n");
	}
	
}

bool board_value_at(board_t* board, unsigned int x, unsigned int y)
{
	assert( false && "NOT IMPLEMENTED" );
}



