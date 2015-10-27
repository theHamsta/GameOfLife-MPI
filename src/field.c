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
			
			bool bWasAlive = (field->val >> FIELD_ELEMENT_SHIFT(x,y)) & 1;
			
			bool bIsAlive = false;
			if( numNeighbours == 3 ) {
				bIsAlive = true;
			} else if ( bWasAlive && numNeighbours == 2 ) {
				bIsAlive = true;
			}
			field->val = bIsAlive << FIELD_ELEMENT_SHIFT(x,y);
			
			field->bitfield.wasChanged = (bIsAlive != bWasAlive);
		}
	}
}

void field_print(field_t* field, unsigned int line)
{
	
	for ( int x = 0; x < BACTERIA_PER_FIELD_X; x++ ) {
		printf("%c", FIELD_GET_ELEMENT(*field, x, line) ? 'X' : '_');
	}
	
}




