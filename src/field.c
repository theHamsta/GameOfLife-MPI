#include "field.h"

#include <stdbool.h>
#include <stdint.h>

// from: http://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer
uint32_t popcount(uint32_t i)
{
	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}


void field_update( field_t* field ) {
	for ( int i = 0; i < BACTERIA_PER_FIELD; i++ ) {
		
		uint32_t neighbours = (field->val >> i) & FIELD_NEIGHBOUR_MASK;
		uint32_t numNeighbours = popcount(neighbours);
		
		bool bWasAlive = (field->bitfield.oldCells >> i) & 1;
		
		bool bIsAlive = false;
		if( numNeighbours == 3 ) {
			bIsAlive = true;
		} else if ( bWasAlive && numNeighbours == 2 ) {
			bIsAlive = true;
		}
				field->bitfield.newCells = bIsAlive << i;
	}
}
