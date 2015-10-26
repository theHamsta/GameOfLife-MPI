#ifndef BOARD_H
#define BOARD_H


#include <stdint.h>

#define BACTERIA_PER_FIELD 4

typedef uint32_t field_t;


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

void board_step_local(board_t* newBoard, board_t* oldBoard);

#endif // BOARD_H