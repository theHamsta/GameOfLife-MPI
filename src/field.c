#include "field.h"

#include <stdio.h>

// from: http://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer
uint32_t popcount(uint32_t i)
{
	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}


void field_update( field_t* field ) {
	

	for ( int y = 0; y < BACTERIA_PER_FIELD_Y; y++ ) {
		for ( int x = 0; x < BACTERIA_PER_FIELD; x++ ) {
			
			uint32_t neighbours = (field->val >> FIELD_ELEMENT_SHIFT_FOR_MASK(x,y)) & FIELD_NEIGHBOUR_MASK;
			uint32_t numNeighbours = popcount(neighbours);
			
			bool bWasAlive = (field->val >> FIELD_ELEMENT_SHIFT_FOR_MASK(x+1,y+1)) & 1;
			
			bool bIsAlive = false;
			if( numNeighbours == 3 ) {
				bIsAlive = true;
			} else if ( bWasAlive && numNeighbours == 2 ) {
				bIsAlive = true;
			}
			field->val = bIsAlive << FIELD_ELEMENT_SHIFT_FOR_MASK(x+1,y+1);
			
			field->bitfield.wasChanged = (bIsAlive != bWasAlive);
		}
	}
}

void field_print(field_t* field, unsigned int line)
{
	
	for ( int x = BACTERIA_PER_FIELD_X - 1; x >= 0; x-- ) {
		printf("%c", FIELD_GET_ELEMENT(*field, x, line) ? 'X' : '_');
	}
	
}

void field_printDebug(field_t* field, int line)
{
	for ( int x = BACTERIA_PER_FIELD_X + 2 - 1; x >= 0; x-- ) {
		

		if( line == 0 || line == BACTERIA_PER_FIELD_Y + 1 || x == 0 || x == BACTERIA_PER_FIELD_X + 1) {
			printf("%c", ((field->val >> FIELD_ELEMENT_SHIFT_FOR_MASK( x, line)) & 1) ? 'o' : '_');
		} else {
			printf("%c", ((field->val >> FIELD_ELEMENT_SHIFT_FOR_MASK(x, line)) & 1) ? 'X' : ' ');
		}
		
	}
	
}

void field_printDebugAllLines(field_t* field)
{
	for( int i = BACTERIA_PER_FIELD_Y + 2 - 1; i >= 0; i-- ) {
		field_printDebug(field, i);
		printf("\n");
	}
}




