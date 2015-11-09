#ifndef BOARD_H
#define BOARD_H


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "field.h"


#define BOARD_PADDING_X (1)
#define BOARD_PADDING_Y (1)
#define BOARD_LINE_SKIP(BOARD) ((BOARD).widthDiv4 + 2 * BOARD_PADDING_X)
#define BOARD_PTR_FIRST_FIELD(BOARD) ((BOARD).data + BOARD_PADDING_Y * BOARD_LINE_SKIP(BOARD) + BOARD_PADDING_X)

#define BOARD_GET_FIELD_PTR(BOARD,X,Y) (&((BOARD)->data[ ((X) + BOARD_PADDING_X) + ((Y) + BOARD_PADDING_Y) * BOARD_LINE_SKIP(*BOARD) ]))



typedef struct board_s
{
	unsigned int widthDiv4;
	unsigned int heightDiv3;
	
	field_t* data;
} board_t;


board_t* board_create(unsigned int widthDiv4, unsigned int heightDiv3 );

void board_reset(board_t* board);

void board_fillRandomly(board_t* board);

void board_print(board_t* board);

void board_printDebug(board_t* board);

void board_destroy(board_t* board);

void board_step(board_t* board);

void board_updateFields(board_t* board);

void board_broadcastNeighbourhoods(board_t* board);

size_t board_getMemoryUsageData(board_t* board);

void board_updateVerticalMargins( board_t* board );

void board_updateHorizontalMargins( board_t* board );

void board_updateCornerFields( board_t* board );

void board_updateInnerFields( board_t* board );


#endif // BOARD_H