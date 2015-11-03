#include "field.h"

#include <stdio.h>
#include <assert.h>

#define BITS_IN_UINT32 (32)

#define FIELD_3X6LINE_LUT_NUM_ENTRIES (1 << (3 * (BACTERIA_PER_FIELD_X + 2/*left and right margin*/)))
#define FIELD_3X6LINE_LUT_SIZE ((FIELD_3X6LINE_LUT_NUM_ENTRIES) / (BITS_IN_UINT32 / BACTERIA_PER_FIELD_X))
// get 4 bit at a time by analysing the corresponding 3x6 neighbourhood 
uint32_t field_3x6line_lut[ FIELD_3X6LINE_LUT_SIZE ];

bool field_lutsInitialised = false;

// from: http://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer
static inline uint32_t popcount(uint32_t i)
{
	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}


static void field_updateNeighbourCount( field_t* field ) {

	field_t newField = *field;

	for ( int y = 0; y < BACTERIA_PER_FIELD_Y; y++ ) {
		for ( int x = 0; x < BACTERIA_PER_FIELD_X; x++ ) {
			
			uint32_t neighbours = (field->val >> (y*FIELD_LINE_WIDTH + x)) & FIELD_NEIGHBOUR_MASK;
			uint32_t numNeighbours = popcount(neighbours);
			
			bool bWasAlive = (field->val >> FIELD_ELEMENT_SHIFT_FOR_MASK(x+1,y+1)) & 1;
			
			bool bIsAlive = false;
			if( numNeighbours == 3 ) {
				bIsAlive = true;
			} else if ( bWasAlive && numNeighbours == 2 ) {
				bIsAlive = true;
			}
			
			newField.val &= ~(1 << FIELD_ELEMENT_SHIFT_FOR_MASK(x+1,y+1));
			newField.val |= bIsAlive << FIELD_ELEMENT_SHIFT_FOR_MASK(x+1,y+1);
			
			newField.bitfield.wasChanged |= (bIsAlive != bWasAlive);
		}
	}
	*field = newField;
}



void field_updateWithLut( field_t* field ) {
	assert ( field_lutsInitialised || "LUTs need to be initialised by field_initLuts!" );
	
	uint32_t lowLutIdx = field->val & FIELD_THREE_LINES_MASK;
	uint32_t midLutIdx = (field->val >> FIELD_LINE_WIDTH) & FIELD_THREE_LINES_MASK;
	uint32_t highLutIdx = (field->val >> (2 * FIELD_LINE_WIDTH)) & FIELD_THREE_LINES_MASK;
	
	
	int idx = lowLutIdx / (BITS_IN_UINT32 / BACTERIA_PER_FIELD_X);
	int shift = (lowLutIdx % (BITS_IN_UINT32 / BACTERIA_PER_FIELD_X)) * BACTERIA_PER_FIELD_X ;
	uint32_t lowRlt = (field_3x6line_lut[ idx ] >> shift ) & FIELD_LINE_WIDTH;
	
	
	idx = midLutIdx / (BITS_IN_UINT32 / BACTERIA_PER_FIELD_X);
	shift = (midLutIdx % (BITS_IN_UINT32 / BACTERIA_PER_FIELD_X)) * BACTERIA_PER_FIELD_X ;
	uint32_t midRlt = (field_3x6line_lut[ idx ] >> shift ) & FIELD_LINE_WIDTH;
	
	idx = highLutIdx / (BITS_IN_UINT32 / BACTERIA_PER_FIELD_X);
	shift = (highLutIdx % (BITS_IN_UINT32 / BACTERIA_PER_FIELD_X)) * BACTERIA_PER_FIELD_X ;
	uint32_t highRlt = (field_3x6line_lut[ idx ] >> shift ) & FIELD_LINE_WIDTH;
	
	field_t newField;
	newField.val = (field->val & ~FIELD_ALL_ELEMENTS_MASK) | (highRlt << (3 * FIELD_LINE_WIDTH + 1)) | (midRlt << (2 * FIELD_LINE_WIDTH + 1)) | (lowRlt <<  FIELD_LINE_WIDTH + 1);
	newField.bitfield.wasChanged = (newField.val != field->val);
	*field = newField;
}


void field_update( field_t* field ) {
	field_t backup = *field;
	field_updateWithLut(field);
	field_updateNeighbourCount(&backup);
	
	assert( backup.val == field->val );
}


void field_initLuts()
{
	uint32_t curResult = 0;
	for ( unsigned int i = 0; i < FIELD_3X6LINE_LUT_NUM_ENTRIES; i++ ) {
		for ( unsigned int x = 0; x < BACTERIA_PER_FIELD_X; x++ ) {
			uint32_t neighbours = (i >>  x) & FIELD_NEIGHBOUR_MASK;
			uint32_t numNeighbours = popcount(neighbours);
			
			bool bWasAlive = (i >> x >> FIELD_LINE_WIDTH) & 1;
			
			bool bIsAlive = false;
			if( numNeighbours == 3 ) {
				bIsAlive = true;
			} else if ( bWasAlive && numNeighbours == 2 ) {
				bIsAlive = true;
			}
			
			curResult |= bIsAlive << ((i % (BITS_IN_UINT32 / BACTERIA_PER_FIELD_X)) * BACTERIA_PER_FIELD_X + x);
		}
		if ( i % (BITS_IN_UINT32 / BACTERIA_PER_FIELD_X) == (BITS_IN_UINT32 / BACTERIA_PER_FIELD_X) - 1 ) {
			field_3x6line_lut[ i / (BITS_IN_UINT32 / BACTERIA_PER_FIELD_X) ] = curResult;
			curResult = 0;
		}
	}
	
	
	field_lutsInitialised = true;
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




