#ifndef BOARD_H
#define BOARD_H


#include <stdint.h>

#define BACTERIA_PER_FIELD 4

typedef uint32_t field_t;


typedef struct board_s
{
	unsigned int m_width;
	unsigned int m_height;
	
	field_t* m_data;
} board_t;


void board_init(board_t* board, unsigned width, unsigned height);

void board_reset(board_t* board);

void board_print(board_t* board);

void board_destroy(board_t* board);

#endif // BOARD_H