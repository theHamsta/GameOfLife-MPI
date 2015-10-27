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
#define FIELD_LEFT_AND_RIGHT_MASK ( 0x5 ) 
#define FIELD_NEIGHBOUR_MASK ( FIELD_ABOVE_MASK || FIELD_BELOW_MASK || FIELD_LEFT_AND_RIGHT_MASK )

#define FIELD_LINE_WIDTH_ONES ((1 << BACTERIA_PER_FIELD_X) - 1)
#define FIELD_ALL_ELEMENTS_MASK ( (FIELD_LINE_WIDTH_ONES << FIELD_ELEMENT_SHIFT(0,0)) | (FIELD_LINE_WIDTH_ONES << FIELD_ELEMENT_SHIFT(0,1)) | (FIELD_LINE_WIDTH_ONES << FIELD_ELEMENT_SHIFT(0,2)))
#define FIELD_NON_ELEMENTS_MASK (~FIELD_ALL_ELEMENTS_MASK)

#define FIELD_ELEMENT_SHIFT(X,Y) ( (1 + X) + (Y + 1) * FIELD_LINE_WIDTH )
#define FIELD_ELEMENT_SHIFT_FOR_MASK(X,Y) ( (X) + (Y) * FIELD_LINE_WIDTH )

#define FIELD_GET_ELEMENT(FIELD, X, Y) (((FIELD).val >> FIELD_ELEMENT_SHIFT((X),(Y))) & 1)

#define FIELD_WAS_CHANGED(FIELD)	((FIELD).bitfield.wasChanged)
#define FIELD_SET_WAS_CHANGED(FIELD) (FIELD_WAS_CHANGED(FIELD) == true)
#define FIELD_UNSET_WAS_CHANGED(FIELD) (FIELD_WAS_CHANGED(FIELD) == false)


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

struct field_s 
{
	unsigned int right0:1;
	unsigned int line0 :5;
	unsigned int left0 :1;
	
	unsigned int right1:1;
	unsigned int line1 :5;
	unsigned int left1 :1;
	
	unsigned int right2:1;
	unsigned int line2 :5;
	unsigned int left2 :1;
	
	unsigned int right3:1;
	unsigned int line3 :5;
	unsigned int left3 :1;
	
	bool wasChanged	   :1;
};

typedef union field_u {
	uint32_t val;
	struct field_s bitfield;
} field_t;




void field_update( field_t* field );

void field_print( field_t* field, unsigned int line );

bool field_has_changed ( field_t* field );



#endif // FIELD_H