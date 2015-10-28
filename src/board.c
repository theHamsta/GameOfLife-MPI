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
	
	size_t bufferSize = board_getMemoryUsageData(board);
	
	board->frontBuffer = malloc( bufferSize ); 
	board->backBuffer = malloc( bufferSize ); 
	
	if ( !board->frontBuffer || !board->backBuffer ) {
		printf("Allocation of board failed\n");
		exit(EXIT_FAILURE);
	}
	
	return board;
}

size_t board_getMemoryUsageData(board_t* board)
{
// 	int fieldsForBoard = board->height * board->height / (BACTERIA_PER_FIELD);
// 	int fieldsPadding = 2 * BOARD_PADDING_Y * ((board->width / BACTERIA_PER_FIELD_X) + 2 * BOARD_PADDING_X) + // padding row above and below field
// 						2 * BOARD_PADDING_X * (board->height / BACTERIA_PER_FIELD_Y); 					   // padding column left and right of field
// 	
// 	return sizeof(field_t) * (fieldsForBoard + fieldsPadding);
	
	return sizeof(field_t) * (board->height / BACTERIA_PER_FIELD_Y + 2 * BOARD_PADDING_Y) * (board->width / BACTERIA_PER_FIELD_X + 2 * BOARD_PADDING_X);
}


void board_destroy(board_t* board)
{
	free(board->frontBuffer);
	free(board->frontBuffer);
	free(board);
}

void board_reset(board_t* board)
{
	assert( board && board->frontBuffer && "null pointer!" );
	int size =  board_getMemoryUsageData(board) ;
	memset((char*)(board->frontBuffer), 0, size );
	
// 	for(int i = 0; i < size/sizeof(field_t); i++){
// 		(board->frontBuffer)[i].val = 0;
// 	}
}

void board_print(board_t* board)
{
	field_t* current = BOARD_PTR_FIRST_FIELD(*board, board->frontBuffer);
	for ( unsigned int y = 0; y < board->height / BACTERIA_PER_FIELD_Y; y++ ) {
		for ( int l = BACTERIA_PER_FIELD_Y - 1; l >= 0; l-- ) {
			for ( unsigned int x = 0; x < board->width / BACTERIA_PER_FIELD_X; x++ ) {

				field_print(&board->frontBuffer[(x+1) + (y+1) * (board->width / BACTERIA_PER_FIELD_X + 2 * BOARD_PADDING_X)], l);
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
	field_t* current = board->frontBuffer;
	for ( unsigned int y = 0; y < board->height / BACTERIA_PER_FIELD_Y + 2 * BOARD_PADDING_Y; y++ ) {
		for ( int l = BACTERIA_PER_FIELD_Y + 2 - 1; l >= 0; l-- ) {
			for ( unsigned int x = 0; x < board->width / BACTERIA_PER_FIELD_X + 2 * BOARD_PADDING_X; x++ ) {
				
 				field_printDebug(&board->frontBuffer[x + y * (board->width / BACTERIA_PER_FIELD_X + 2 * BOARD_PADDING_X)], l);
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
	field_t field = board->frontBuffer[ (y * board->width + x) / BACTERIA_PER_FIELD ];
	return (field.bitfield.oldCells >> (BACTERIA_PER_FIELD - 1 - (y % BACTERIA_PER_FIELD))) & 1;
}*/




void board_step( board_t* board )
{
	field_t* aboveNew = board->frontBuffer + BOARD_PADDING_X;
	field_t* current = BOARD_PTR_FIRST_FIELD(*board, board->frontBuffer);
	field_t* currentNew = BOARD_PTR_FIRST_FIELD(*board, board->backBuffer);
	field_t* belowNew = currentNew + BOARD_LINE_SKIP(*board);
	
	
	for ( unsigned int y = 0; y < board->height / BACTERIA_PER_FIELD_Y; y++ ) {
		for( unsigned int x = 0; x < board->width / BACTERIA_PER_FIELD_X; x++ ) {
			
			if(current->val != 0) {
				field_update(current, currentNew);
			}
			
			if(FIELD_WAS_CHANGED(*current)) {
				field_broadcastLeft			(currentNew, currentNew - 1);
				field_broadcastTopLeft		(currentNew, aboveNew - 1);
				field_broadcastTop			(currentNew, aboveNew);
				field_broadcastTopRight		(currentNew, aboveNew + 1);
				field_broadcastRight		(currentNew, currentNew + 1);
				field_broadcastBottomRight	(currentNew, belowNew + 1);
				field_broadcastBottom		(currentNew, belowNew);
				field_broadcastBottomLeft	(currentNew, belowNew - 1);
				
				FIELD_UNSET_WAS_CHANGED(*currentNew);
			}
			
			
			
			aboveNew++;
			current++;
			currentNew++;
			belowNew++;
		}
		aboveNew += 2 * BOARD_PADDING_X;
		current += 2 * BOARD_PADDING_X;
		currentNew += 2 * BOARD_PADDING_X;
		belowNew += 2 * BOARD_PADDING_X;
	}

	board_bufferSwitch(board);
}

void board_fillRandomly(board_t* board)
{
	board_reset(board);
	
// 	srand(time(NULL));
	srand(14);
	
	int memlimit = board_getMemoryUsageData(board) / sizeof(field_t);
	
	field_t* above = board->frontBuffer + BOARD_PADDING_X;
	field_t* current = BOARD_PTR_FIRST_FIELD(*board, board->frontBuffer);
	field_t* below = current + BOARD_LINE_SKIP(*board);
	
	for ( unsigned int y = 0; y < board->height / BACTERIA_PER_FIELD_Y; y++ ) {
		for ( unsigned int x = 0; x < board->width / BACTERIA_PER_FIELD_X; x++ ) {
			int rnd = rand();
		
// 			printf("%i", board->frontBuffer[(x+1) + (y+1)* (board->width / BACTERIA_PER_FIELD_X +2*BOARD_PADDING_X)].val);
// 			board->frontBuffer[(x+1) + (y+1)* (board->width / BACTERIA_PER_FIELD_X + 2 * BOARD_PADDING_X)].val = rnd & FIELD_ALL_ELEMENTS_MASK;
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


void board_bufferSwitch(board_t* board)
{	
	field_t* tmp = board->frontBuffer;
	board->frontBuffer = board->backBuffer;
	board->backBuffer = tmp;
}

