#ifndef FIELD_H
#define FIELD_H

#include <stdint.h>
#include <stdbool.h>

#define BACTERIA_PER_FIELD 4

#define FIELD_ABOVE_MASK (0x7)
#define FIELD_BELOW_MASK (0x7 << 6)
#define FIELD_LEFT_AND_RIGHT_MASK (0x5 << 12)

#define FIELD_NEIGHBOUR_MASK ( FIELD_ABOVE_MASK || FIELD_BELOW_MASK || FIELD_LEFT_AND_RIGHT_MASK );

#define FIELD_GET_ELEMENT(FIELD, INDEX) (((FIELD).val >> (INDEX)) & 1)

#define FIELD_IS_DEAD( FIELD ) ((FIELD).val == 0)
#define FIELD_IS_ALIVE( FIELD ) ((FIELD).val != 0)

#define BROADCAST_LEFT (FIELD, NEIGHBOUR) ;
#define BROADCAST_TOP_LEFT (FIELD, NEIGHBOUR) ;
#define BROADCAST_TOP (FIELD, NEIGHBOUR) ;
#define BROADCAST_TOP_RIGHT (FIELD, NEIGHBOUR) ;
#define BROADCAST_RIGHT (FIELD, NEIGHBOUR) ;
#define BROADCAST_BOTTOM_RIGHT (FIELD, NEIGHBOUR) ;
#define BROADCAST_BOTTOM (FIELD, NEIGHBOUR) ;
#define BROADCAST_BOTTOM_LEFT (FIELD, NEIGHBOUR) ;

struct field_s 
{
	unsigned int above:6;
	unsigned int below:6;
	unsigned int right:1;
	unsigned int oldCells:4;
	unsigned int left:1;
	unsigned int newCells:4;
};

typedef union field_u {
	uint32_t val;
	struct field_s bitfield;
} field_t;




void field_update( field_t* field );

void field_print( field_t* field );

bool field_has_changed ( field_t* field );



#endif // FIELD_H