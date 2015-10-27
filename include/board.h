#ifndef BOARD_H
#define BOARD_H


#include <stdint.h>
#include <stdbool.h>

#include "field.h"





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