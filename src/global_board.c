#include "global_board.h"


void global_board_destroy(global_board_t* board)
{
	board_destroy(board->local_board);
	free(board);
}
