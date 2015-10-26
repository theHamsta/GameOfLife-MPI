#ifndef BOARD_H
#define BOARD_H


#include <stdint.h>
#include <stdbool.h>

#define BACTERIA_PER_FIELD 4




#define FIELD_ABOVE_MASK (0x7)
#define FIELD_BELOW_MASK (0x7 << 6)
#define FIELD_LEFT_AND_RIGHT_MASK (0x5 << 12)

#define FIELD_NEIGHBOUR_MASK ( FIELD_ABOVE_MASK || FIELD_BELOW_MASK || FIELD_LEFT_AND_RIGHT_MASK );

typedef struct field_s 
{
	unsigned int above:6;
	unsigned int below:6;
	unsigned int right:1;
	unsigned int oldCells:4;
	unsigned int left:1;
	unsigned int newCells:4;
} field_t;



typedef struct board_s
{
	unsigned int width;
	unsigned int height;
	
	field_t* data;
} board_t;


board_t* board_create(unsigned width, unsigned height);

void board_reset(board_t* board);

void board_print(board_t* board);

void board_destroy(board_t* board);

void board_step(board_t* board);

bool board_value_at( board_t* board, unsigned int x, unsigned int y );


#endif // BOARD_H