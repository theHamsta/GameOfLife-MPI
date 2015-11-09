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


board_t* board_create(unsigned int widthDiv4, unsigned int heightDiv3 )
{
	board_t* board = malloc(sizeof(board_t));
	
	assert(board);
	
	board->heightDiv3 = heightDiv3;
	board->widthDiv4 = widthDiv4;
	
	
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
		* (board->heightDiv3 + 2 * BOARD_PADDING_Y)
		* (board->widthDiv4 + 2 * BOARD_PADDING_X);
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
	for ( unsigned int y = 0; y < board->heightDiv3; y++ ) {
		for ( int l = BACTERIA_PER_FIELD_Y - 1; l >= 0; l-- ) {
			for ( unsigned int x = 0; x < board->widthDiv4; x++ ) {

				field_print(&board->data[(x+1) + (y+1) * (board->widthDiv4 + 2 * BOARD_PADDING_X)], l);
				current++;
			}
			current -= board->widthDiv4;
			printf("\n");
		}
		current += 2 * BOARD_PADDING_X + board->widthDiv4 * BACTERIA_PER_FIELD_X;

	}
	printf("\n");
}

void board_printDebug(board_t* board)
{
	field_t* current = board->data;
	for ( unsigned int y = 0; y < board->heightDiv3 + 2 * BOARD_PADDING_Y; y++ ) {
		for ( int l = BACTERIA_PER_FIELD_Y + 2 - 1; l >= 0; l-- ) {
			for ( unsigned int x = 0; x < board->widthDiv4 + 2 * BOARD_PADDING_X; x++ ) {
				
 				field_printDebug(&board->data[x + y * (board->widthDiv4 + 2 * BOARD_PADDING_X)], l);
				current++;
			}
			current -=  board->widthDiv4 + 2 * BOARD_PADDING_X;
			printf("\n");
		}
		current += 2 * BOARD_PADDING_X +  board->widthDiv4 * BACTERIA_PER_FIELD_X;;

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
	
	
	for ( unsigned int y = 0; y < board->heightDiv3; y++ ) {
		for( unsigned int x = 0; x < board->widthDiv4; x++ ) {
			
			
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
	
	
	for ( unsigned int y = 0; y < board->heightDiv3; y++ ) {
		for( unsigned int x = 0; x < board->widthDiv4; x++ ) {
			
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

	
	
	for ( unsigned int y = 1; y < board->heightDiv3 - 1; y++ ) {
		for( unsigned int x = 1; x < board->widthDiv4 - 1; x++ ) {
			
			field_t *cur = BOARD_GET_FIELD_PTR(board, x, y);
			
			if(cur->val != 0) {
				field_update(cur);
			}
		}
	}
}


void board_updateCornerFields(board_t* board)
{
	field_t* topLeft = BOARD_GET_FIELD_PTR(board, 0, 0);
	field_t* topRight = BOARD_GET_FIELD_PTR(board, board->widthDiv4 - 1, 0);
	field_t* bottomLeft = BOARD_GET_FIELD_PTR(board, 0, board->heightDiv3 - 1);
	field_t* bottomRight = BOARD_GET_FIELD_PTR(board, board->widthDiv4 - 1, board->heightDiv3 -1);
	
	if( topLeft->val ) {
		field_update( topLeft );
	}
	if( topRight->val ) {
		field_update( topRight );
	}
	if( bottomLeft->val ) {
		field_update( bottomLeft );
	}
	if( bottomRight->val ) {
		field_update( bottomRight );
	}
}



void board_updateHorizontalMargins(board_t* board)
{
	for ( int x = 1; x < board->widthDiv4 - 1; x++ ) {
		field_t* curTop =  BOARD_GET_FIELD_PTR(board, x, 0); 
		field_t* curBottom = BOARD_GET_FIELD_PTR(board, x, board->heightDiv3 - 1); 
		
		if(curTop->val){
			field_update(curTop);
		}
		if(curBottom->val){
			field_update(curBottom);
		}
	}
}

void board_updateVerticalMargins(board_t* board)
{
	for ( int y = 1; y < board->heightDiv3 - 1; y++ ) {
		field_t* curLeft = BOARD_GET_FIELD_PTR(board, 0, y); 
		field_t* curRight = BOARD_GET_FIELD_PTR(board, board->widthDiv4 - 1, y); 
		
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
	
	for ( unsigned int y = 0; y < board->heightDiv3; y++ ) {
		for ( unsigned int x = 0; x < board->widthDiv4; x++ ) {
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


