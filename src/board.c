#include "board.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


void field_broadcastLeft( field_t* field, field_t* neighbour );
void field_broadcastTopLeft( field_t* field, field_t* neighbour );
void field_broadcastTop( field_t* field, field_t* neighbour );
void field_broadcastTopRight( field_t* field, field_t* neighbour );
void field_broadcastRight( field_t* field, field_t* neighbour );
void field_broadcastBottomRight( field_t* field, field_t* neighbour );
void field_broadcastBottom( field_t* field, field_t* neighbour );
void field_broadcastBottomLeft( field_t* field, field_t* neighbour );


board_t* board_create(unsigned int width, unsigned int height)
{
	board_t* board = malloc(sizeof(board_t));
	
	assert(board);
	
	board->height = height;
	board->width = width;
	
	assert ( width % BACTERIA_PER_FIELD_X == 0 && "width must be divisible by BACTERIA_PER_FIELD_X" );
	assert ( height % BACTERIA_PER_FIELD_Y == 0 && "height must be divisible by BACTERIA_PER_FIELD_Y" );
	
	board->data = malloc( board_getMemoryUsageData(board) ); 
	
	if ( !board->data ) {
		printf("Allocation of board failed\n");
		exit(EXIT_FAILURE);
	}
	
	return board;
}

size_t board_getMemoryUsageData(board_t* board)
{	
	return sizeof(field_t)
		* (board->height / BACTERIA_PER_FIELD_Y + 2 * BOARD_PADDING_Y)
		* (board->width / BACTERIA_PER_FIELD_X + 2 * BOARD_PADDING_X);
}


void board_destroy(board_t* board)
{
	free(board->data);
	free(board);
}

void board_reset(board_t* board)
{
	assert( board && board->data && "null pointer!" );
	int size =  board_getMemoryUsageData(board) ;
	memset((char*)(board->data), 0, size );
	
// 	for(int i = 0; i < size/sizeof(field_t); i++){
// 		(board->data)[i].val = 0;
// 	}
}

void board_print(board_t* board)
{
	field_t* current = BOARD_PTR_FIRST_FIELD(*board);
	for ( unsigned int y = 0; y < board->height / BACTERIA_PER_FIELD_Y; y++ ) {
		for ( int l = BACTERIA_PER_FIELD_Y - 1; l >= 0; l-- ) {
			for ( unsigned int x = 0; x < board->width / BACTERIA_PER_FIELD_X; x++ ) {

				field_print(&board->data[(x+1) + (y+1) * (board->width / BACTERIA_PER_FIELD_X + 2 * BOARD_PADDING_X)], l);
				current++;
			}
			current -= board->width / BACTERIA_PER_FIELD_X;
			printf("\n");
		}
		current += 2 * BOARD_PADDING_X + board->width;

	}
	printf("\n");
}

void board_printDebug(board_t* board)
{
	field_t* current = board->data;
	for ( unsigned int y = 0; y < board->height / BACTERIA_PER_FIELD_Y + 2 * BOARD_PADDING_Y; y++ ) {
		for ( int l = BACTERIA_PER_FIELD_Y + 2 - 1; l >= 0; l-- ) {
			for ( unsigned int x = 0; x < board->width / BACTERIA_PER_FIELD_X + 2 * BOARD_PADDING_X; x++ ) {
				
 				field_printDebug(&board->data[x + y * (board->width / BACTERIA_PER_FIELD_X + 2 * BOARD_PADDING_X)], l);
				current++;
			}
			current -= board->width + 2 * BOARD_PADDING_X;
			printf("\n");
		}
		current += 2 * BOARD_PADDING_X + board->width;;

	}
	printf("\n");
}


/*
bool board_value_at(board_t* board, unsigned int x, unsigned int y)
{
	field_t field = board->data[ (y * board->width + x) / BACTERIA_PER_FIELD ];
	return (field.bitfield.oldCells >> (BACTERIA_PER_FIELD - 1 - (y % BACTERIA_PER_FIELD))) & 1;
}*/



void board_broadcastNeighbourhoods(board_t* board)
{
	field_t* above = board->data + BOARD_PADDING_X;
	field_t* current = BOARD_PTR_FIRST_FIELD(*board);
	field_t* below = current + BOARD_LINE_SKIP(*board);
	
	
	for ( unsigned int y = 0; y < board->height / BACTERIA_PER_FIELD_Y; y++ ) {
		for( unsigned int x = 0; x < board->width / BACTERIA_PER_FIELD_X; x++ ) {
			
			
			if(FIELD_WAS_CHANGED(*current)) {
				field_broadcastLeft			(current, current - 1);
				field_broadcastTopLeft		(current, above - 1);
				field_broadcastTop			(current, above);
				field_broadcastTopRight		(current, above + 1);
				field_broadcastRight		(current, current + 1);
				field_broadcastBottomRight	(current, below + 1);
				field_broadcastBottom		(current, below);
				field_broadcastBottomLeft	(current, below - 1);
				
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

void board_updateFields(board_t* board)
{
	field_t* current = BOARD_PTR_FIRST_FIELD(*board);
	
	
	for ( unsigned int y = 0; y < board->height / BACTERIA_PER_FIELD_Y; y++ ) {
		for( unsigned int x = 0; x < board->width / BACTERIA_PER_FIELD_X; x++ ) {
			
			if(current->val != 0) {
				field_update(current);
			}
			current++;
		}
		current += 2 * BOARD_PADDING_X;
	}
}

void board_updateInnerFields(board_t* board)
{

	
	
	for ( unsigned int y = 1; y < board->height / BACTERIA_PER_FIELD_Y - 1; y++ ) {
		for( unsigned int x = 1; x < board->width / BACTERIA_PER_FIELD_X - 1; x++ ) {
			
			field_t *cur = &board->data[ x + BOARD_PADDING_X + (y+BOARD_PADDING_Y) * BOARD_LINE_SKIP(*board)];
			
			if(cur->val != 0) {
				field_update(cur);
			}
		}
	}
}

void board_updateOuterFields(board_t* board)
{
	field_t* current = BOARD_PTR_FIRST_FIELD(*board);
	
	for ( int x = 1; x < board->width / BACTERIA_PER_FIELD_X - 1; x++ ) {
		field_t* curTop = &board->data[ BOARD_PADDING_X + x + 0 * BOARD_LINE_SKIP(*board) ];
		field_t* curBottom = &board->data[ BOARD_PADDING_X + x + board->height / BACTERIA_PER_FIELD_Y * BOARD_LINE_SKIP(*board)   ];
		
		if(curTop->val){
			field_update(curTop);
		}
		if(curBottom->val){
			field_update(curBottom);
		}
	}
	
	
	for ( int y = 0; y < board->height / BACTERIA_PER_FIELD_Y; y++ ) {
		field_t* curLeft = &board->data[ BOARD_PADDING_Y + (y + BOARD_PADDING_Y) * BOARD_LINE_SKIP(*board) ];
		field_t* curRight = &board->data[ board->width + (y  + BOARD_PADDING_Y) * BOARD_LINE_SKIP(*board)  ];
		
		if(curLeft->val){
			field_update(curLeft);
		}
		if(curRight->val){
			field_update(curRight);
		}
	}
}




void board_fillRandomly(board_t* board)
{
	board_reset(board);

	
	field_t* above = board->data + BOARD_PADDING_X;
	field_t* current = BOARD_PTR_FIRST_FIELD(*board);
	field_t* below = current + BOARD_LINE_SKIP(*board);
	
	for ( unsigned int y = 0; y < board->height / BACTERIA_PER_FIELD_Y; y++ ) {
		for ( unsigned int x = 0; x < board->width / BACTERIA_PER_FIELD_X; x++ ) {
			int rnd = rand();

			current->val |= rnd & FIELD_ALL_ELEMENTS_MASK;
			
			field_broadcastLeft			(current, current -1);
			field_broadcastTopLeft		(current, above - 1);
			field_broadcastTop			(current, above);
			field_broadcastTopRight		(current, above + 1);
			field_broadcastRight		(current, current + 1);
			field_broadcastBottomRight	(current, below + 1);
			field_broadcastBottom		(current, below);
			field_broadcastBottomLeft	(current, below - 1);
			
			above++;
			current++;
			below++;
			
		}
		above += 2 * BOARD_PADDING_X;
		current += 2 * BOARD_PADDING_X;
		below += 2 * BOARD_PADDING_X;
	}
	
}

bool board_value_at(board_t* board, unsigned int x, unsigned int y)
{
	assert( false && "NOT IMPLEMENTED" );
}

void board_step(board_t* board)
{
	
	board_updateFields(board);
	board_broadcastNeighbourhoods(board);
	
}


