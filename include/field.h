#ifndef FIELD_H
#define FIELD_H

#include <stdint.h>
#include <stdbool.h>


#define BACTERIA_PER_FIELD   (9)
#define BACTERIA_PER_FIELD_X (4)
#define BACTERIA_PER_FIELD_Y (3)

#define FIELD_LINE_WIDTH  ((BACTERIA_PER_FIELD_X) + 2)
#define FIELD_ABOVE_MASK ( 0x7 << (2*FIELD_LINE_WIDTH))
#define FIELD_BELOW_MASK ( 0x7 ) 
#define FIELD_LEFT_AND_RIGHT_MASK ( 0x5 << (FIELD_LINE_WIDTH)) 
#define FIELD_NEIGHBOUR_MASK ( FIELD_ABOVE_MASK | FIELD_BELOW_MASK | FIELD_LEFT_AND_RIGHT_MASK )

#define FIELD_LINE_WIDTH_ONES ((1 << BACTERIA_PER_FIELD_X) - 1)
#define FIELD_ALL_ELEMENTS_MASK ( (FIELD_LINE_WIDTH_ONES << FIELD_ELEMENT_SHIFT_FOR_MASK(1,1)) | (FIELD_LINE_WIDTH_ONES << FIELD_ELEMENT_SHIFT_FOR_MASK(1,2)) | (FIELD_LINE_WIDTH_ONES << FIELD_ELEMENT_SHIFT_FOR_MASK(1,3)))
#define FIELD_NON_ELEMENTS_MASK (~FIELD_ALL_ELEMENTS_MASK)

//#define FIELD_ELEMENT_SHIFT(X,Y) ( (1 + X) + (Y + 1) * FIELD_LINE_WIDTH )
#define FIELD_ELEMENT_SHIFT_FOR_MASK(X,Y) ( (X) + (Y) * FIELD_LINE_WIDTH )

#define FIELD_GET_ELEMENT(FIELD, X, Y) (((FIELD).val >> FIELD_ELEMENT_SHIFT_FOR_MASK((X+1),(Y+1))) & 1)

#define FIELD_WAS_CHANGED(FIELD)	((FIELD).bitfield.wasChanged)
#define FIELD_SET_WAS_CHANGED(FIELD) (FIELD_WAS_CHANGED(FIELD) = true)
#define FIELD_UNSET_WAS_CHANGED(FIELD) (FIELD_WAS_CHANGED(FIELD) = false)


// #define BACTERIA_PER_FIELD 4
// 
// #define FIELD_ABOVE_MASK (0x7)
// #define FIELD_BELOW_MASK (0x7 << 6)
// #define FIELD_LEFT_AND_RIGHT_MASK (0x5 << 12)
// 
// #define FIELD_NEIGHBOUR_MASK ( FIELD_ABOVE_MASK || FIELD_BELOW_MASK || FIELD_LEFT_AND_RIGHT_MASK );
// 
// #define FIELD_GET_ELEMENT(FIELD, INDEX) (((FIELD).val >> (INDEX)) & 1)



#define BROADCAST_LEFT (FIELD, NEIGHBOUR) 
#define BROADCAST_TOP_LEFT (FIELD, NEIGHBOUR) 
#define BROADCAST_TOP (FIELD, NEIGHBOUR) 
#define BROADCAST_TOP_RIGHT (FIELD, NEIGHBOUR) 
#define BROADCAST_RIGHT (FIELD, NEIGHBOUR) 
#define BROADCAST_BOTTOM_RIGHT (FIELD, NEIGHBOUR) 
#define BROADCAST_BOTTOM (FIELD, NEIGHBOUR) 
#define BROADCAST_BOTTOM_LEFT (FIELD, NEIGHBOUR) 

#define FIELD_ALL_NEIGHBOURS_LEFT_MASK ( (1 << FIELD_ELEMENT_SHIFT_FOR_MASK(BACTERIA_PER_FIELD_X + 1,1)) | (1 << FIELD_ELEMENT_SHIFT_FOR_MASK(BACTERIA_PER_FIELD_X + 1,2)) | (1 << FIELD_ELEMENT_SHIFT_FOR_MASK(BACTERIA_PER_FIELD_X + 1,3)))
#define FIELD_ALL_NEIGHBOURS_TOP_LEFT_MASK (1 << FIELD_ELEMENT_SHIFT_FOR_MASK(BACTERIA_PER_FIELD_X + 1, BACTERIA_PER_FIELD_Y + 1)) 
#define FIELD_ALL_NEIGHBOURS_TOP_MASK ( ((1<<(BACTERIA_PER_FIELD_X))-1) << FIELD_ELEMENT_SHIFT_FOR_MASK(1,BACTERIA_PER_FIELD_Y+1)) 
#define FIELD_ALL_NEIGHBOURS_TOP_RIGHT_MASK ( 1 << FIELD_ELEMENT_SHIFT_FOR_MASK(0,BACTERIA_PER_FIELD_Y+1)) 
#define FIELD_ALL_NEIGHBOURS_RIGHT_MASK ( (1 << FIELD_ELEMENT_SHIFT_FOR_MASK(0,1)) | (1 << FIELD_ELEMENT_SHIFT_FOR_MASK(0,2)) | (1 << FIELD_ELEMENT_SHIFT_FOR_MASK(0,3)))
#define FIELD_ALL_NEIGHBOURS_BOTTOM_RIGHT_MASK ( 1 << FIELD_ELEMENT_SHIFT_FOR_MASK(0,0)) 
#define FIELD_ALL_NEIGHBOURS_BOTTOM_MASK (((1<<(BACTERIA_PER_FIELD_X))-1) << FIELD_ELEMENT_SHIFT_FOR_MASK(1,0)) 
#define FIELD_ALL_NEIGHBOURS_BOTTOM_LEFT_MASK ( 1 << FIELD_ELEMENT_SHIFT_FOR_MASK(BACTERIA_PER_FIELD_X + 1,0))

#define FIELD_ALL_ELEMENTS_LEFT_MASK ( (1 << FIELD_ELEMENT_SHIFT_FOR_MASK(BACTERIA_PER_FIELD_X,1)) | (1 << FIELD_ELEMENT_SHIFT_FOR_MASK(BACTERIA_PER_FIELD_X,2)) | (1 << FIELD_ELEMENT_SHIFT_FOR_MASK(BACTERIA_PER_FIELD_X,3)))
#define FIELD_ALL_ELEMENTS_TOP_LEFT_MASK (1 << FIELD_ELEMENT_SHIFT_FOR_MASK(BACTERIA_PER_FIELD_X,BACTERIA_PER_FIELD_Y)) 
#define FIELD_ALL_ELEMENTS_TOP_MASK ( ((1<<(BACTERIA_PER_FIELD_X))-1) << FIELD_ELEMENT_SHIFT_FOR_MASK(1,BACTERIA_PER_FIELD_Y)) 
#define FIELD_ALL_ELEMENTS_TOP_RIGHT_MASK ( 1 << FIELD_ELEMENT_SHIFT_FOR_MASK(1,BACTERIA_PER_FIELD_Y)) 
#define FIELD_ALL_ELEMENTS_RIGHT_MASK ( (1 << FIELD_ELEMENT_SHIFT_FOR_MASK(1,1)) | (1 << FIELD_ELEMENT_SHIFT_FOR_MASK(1,2)) | (1 << FIELD_ELEMENT_SHIFT_FOR_MASK(1,3)))
#define FIELD_ALL_ELEMENTS_BOTTOM_RIGHT_MASK ( 1 << FIELD_ELEMENT_SHIFT_FOR_MASK(1,1)) 
#define FIELD_ALL_ELEMENTS_BOTTOM_MASK (((1<<(BACTERIA_PER_FIELD_X))-1) << FIELD_ELEMENT_SHIFT_FOR_MASK(1,1)) 
#define FIELD_ALL_ELEMENTS_BOTTOM_LEFT_MASK ( 1 << FIELD_ELEMENT_SHIFT_FOR_MASK(BACTERIA_PER_FIELD_X,1))

#define FIELD_ELEMENT_BL_POS FIELD_ELEMENT_SHIFT_FOR_MASK(BACTERIA_PER_FIELD_X,1)
#define FIELD_ELEMENT_BR_POS FIELD_ELEMENT_SHIFT_FOR_MASK(1,1)
#define FIELD_ELEMENT_TL_POS FIELD_ELEMENT_SHIFT_FOR_MASK(BACTERIA_PER_FIELD_X,BACTERIA_PER_FIELD_Y)
#define FIELD_ELEMENT_TR_POS FIELD_ELEMENT_SHIFT_FOR_MASK(1,BACTERIA_PER_FIELD_Y)

#define FIELD_NEIGHBOUR_BL_POS FIELD_ELEMENT_SHIFT_FOR_MASK(BACTERIA_PER_FIELD_X + 1,0)
#define FIELD_NEIGHBOUR_BR_POS FIELD_ELEMENT_SHIFT_FOR_MASK(0,0)
#define FIELD_NEIGHBOUR_TL_POS FIELD_ELEMENT_SHIFT_FOR_MASK(BACTERIA_PER_FIELD_X + 1, BACTERIA_PER_FIELD_Y + 1)
#define FIELD_NEIGHBOUR_TR_POS FIELD_ELEMENT_SHIFT_FOR_MASK(0,BACTERIA_PER_FIELD_Y + 1)

struct field_s 
{
	unsigned int right0:1;
	unsigned int line0 :BACTERIA_PER_FIELD_X;
	unsigned int left0 :1;
	
	unsigned int right1:1;
	unsigned int line1 :BACTERIA_PER_FIELD_X;
	unsigned int left1 :1;
	
	unsigned int right2:1;
	unsigned int line2 :BACTERIA_PER_FIELD_X;
	unsigned int left2 :1;
	
	unsigned int right3:1;
	unsigned int line3 :BACTERIA_PER_FIELD_X;
	unsigned int left3 :1;
	
	unsigned int right4:1;
	unsigned int line4 :BACTERIA_PER_FIELD_X;
	unsigned int left4 :1;
	
	
	bool wasChanged	   :1;
};

typedef union field_u {
	uint32_t val;
	struct field_s bitfield;
} field_t;




void field_update( field_t* field );

void field_print( field_t* field, unsigned int line );

void field_printDebug( field_t* field, int line );

void field_printDebugAllLines( field_t* field );

bool field_has_changed ( field_t* field );

void inline field_broadcastLeft( field_t* field, field_t* neighbour )
{
	// clear right area of neighbour
	neighbour->val &= UINT32_MAX & ~FIELD_ALL_NEIGHBOURS_RIGHT_MASK;
	// broadcast value
	uint32_t val = (field->val & FIELD_ALL_ELEMENTS_LEFT_MASK) >> BACTERIA_PER_FIELD_X;
	// set value
	neighbour->val |= val;	
}

void inline field_broadcastTopLeft( field_t* field, field_t* neighbour )
{
	// clear right area of neighbour
	neighbour->val &= UINT32_MAX & ~FIELD_ALL_NEIGHBOURS_BOTTOM_RIGHT_MASK;
	// broadcast value
	uint32_t val = (field->val & FIELD_ALL_ELEMENTS_TOP_LEFT_MASK) >> (FIELD_ELEMENT_TL_POS - FIELD_NEIGHBOUR_BR_POS);
	// set value
	neighbour->val |= val;	
}


void inline field_broadcastTop( field_t* field, field_t* neighbour )
{
	// clear right area of neighbour
	neighbour->val &= UINT32_MAX & ~FIELD_ALL_NEIGHBOURS_BOTTOM_MASK;
	// broadcast value
	uint32_t val = (field->val & FIELD_ALL_ELEMENTS_TOP_MASK) >> (BACTERIA_PER_FIELD_Y * FIELD_LINE_WIDTH);
	// set value
	neighbour->val |= val;	
}

void inline field_broadcastTopRight( field_t* field, field_t* neighbour )
{
	// clear right area of neighbour
	neighbour->val &= UINT32_MAX & ~FIELD_ALL_NEIGHBOURS_BOTTOM_LEFT_MASK;
	// broadcast value
	uint32_t val = (field->val & FIELD_ALL_ELEMENTS_TOP_RIGHT_MASK) >> (FIELD_ELEMENT_TR_POS - FIELD_NEIGHBOUR_BL_POS);
	// set value
	neighbour->val |= val;	
}

void inline field_broadcastRight( field_t* field, field_t* neighbour )
{
	// clear right area of neighbour
	neighbour->val &= UINT32_MAX & ~FIELD_ALL_NEIGHBOURS_LEFT_MASK;
	// broadcast value
	uint32_t val = (field->val & FIELD_ALL_ELEMENTS_RIGHT_MASK) << BACTERIA_PER_FIELD_X;
	// set value
	neighbour->val |= val;	
}

void inline field_broadcastBottomRight( field_t* field, field_t* neighbour )
{
	// clear right area of neighbour
	neighbour->val &= UINT32_MAX & ~FIELD_ALL_NEIGHBOURS_TOP_LEFT_MASK;
	// broadcast value
	uint32_t val = (field->val & FIELD_ALL_ELEMENTS_BOTTOM_RIGHT_MASK) << (-FIELD_ELEMENT_BR_POS + FIELD_NEIGHBOUR_TL_POS);
	// set value
	neighbour->val |= val;	
}

void inline field_broadcastBottom( field_t* field, field_t* neighbour )
{
	// clear right area of neighbour
	neighbour->val &= UINT32_MAX & ~FIELD_ALL_NEIGHBOURS_TOP_MASK;
	// broadcast value
	uint32_t val = (field->val & FIELD_ALL_ELEMENTS_BOTTOM_MASK) << (BACTERIA_PER_FIELD_Y * FIELD_LINE_WIDTH);

	// set value
	neighbour->val |= val;	
}

void inline field_broadcastBottomLeft( field_t* field, field_t* neighbour )
{
	// clear right area of neighbour
	neighbour->val &= UINT32_MAX & ~FIELD_ALL_NEIGHBOURS_TOP_RIGHT_MASK;
	// broadcast value
	uint32_t val = (field->val & FIELD_ALL_ELEMENTS_BOTTOM_LEFT_MASK) << (-FIELD_ELEMENT_BL_POS + FIELD_NEIGHBOUR_TR_POS);
	// set value
	neighbour->val |= val;	
}

#endif // FIELD_H