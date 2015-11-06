#include "global_board.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "log.h"



#define GBOARD_UP_BUF_SIZE(BOARD) (  sizeof(field_t) * ((BOARD).widthDiv4 )) 
#define GBOARD_DOWN_BUF_SIZE(BOARD) GBOARD_UP_BUF_SIZE(BOARD)
#define GBOARD_LEFT_BUF_SIZE(BOARD) ( sizeof(field_t) * ((BOARD).heightDiv3))
#define GBOARD_RIGHT_BUF_SIZE(BOARD) GBOARD_LEFT_BUF_SIZE(BOARD)


#define GBOARD_UP_BUF_ELEMENTS(BOARD) (  ((BOARD).widthDiv4)) 
#define GBOARD_DOWN_BUF_ELEMENTS(BOARD) GBOARD_UP_BUF_ELEMENTS(BOARD)
#define GBOARD_LEFT_BUF_ELEMENTS(BOARD) ( (BOARD).heightDiv3)
#define GBOARD_RIGHT_BUF_ELEMENTS(BOARD) GBOARD_LEFT_BUF_ELEMENTS(BOARD)



#ifdef CHECK_RESULTS
#define PERIODIC_BOUNDARY (false)
#else
#define PERIODIC_BOUNDARY (true)
#endif


typedef enum messageDirection_e{
	DIRECTION_UP, DIRECTION_DOWN, DIRECTION_LEFT, DIRECTION_RIGHT, DIRECTION_TL, DIRECTION_TR, DIRECTION_BL, DIRECTION_BR
} messageDirection_t;

typedef enum courners_e{
	COURNER_TL, COURNER_TR, COURNER_BL, COURNER_BR
} courners_t;





globalBoard_t* globalBoard_create(unsigned int width, unsigned int height,  int rank, int numRanksX, int numRanksY )
{
	
	assert( rank < numRanksX * numRanksY );
	
	globalBoard_t* global_board = malloc(sizeof(globalBoard_t));
	
	int dims[2];
	dims[0] = numRanksX;
	dims[1] = numRanksY;
	int periodic[2] = { true, true };
	int reorder = true;
	int coords[2];
	

	assert( dims [0] > 1 );
	assert( dims [1] > 1 );
	
	MPI_Comm comm;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodic , reorder, &comm);
	MPI_Cart_coords(comm, rank, 2, coords);

	// Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

	
	global_board->global_width = width;
	global_board->global_height = height;
	
	assert ( numRanksX * (width / numRanksX) + (width % numRanksX) == width );
	assert ( (width % numRanksX) == 0 );
	assert ( (height % numRanksY) == 0 );
	
	int localWidth = (coords[0] == numRanksX) ? (width / numRanksX) : (width / numRanksX) + (width % numRanksX);
	int localHeight = (coords[1] == numRanksY) ? (height / numRanksY) : (height / numRanksY) + (height % numRanksY);
	
	global_board->local_board = board_create(localWidth, localHeight);
	global_board->local_x = coords[0] * (width / numRanksX);
	global_board->local_y = coords[1] * (height / numRanksY);
	
	global_board->mpi_rankX = coords[0];
	global_board->mpi_rankY = coords[1];
	global_board->mpi_sizeX = numRanksX;
	global_board->mpi_sizeY = numRanksY;
	
	global_board->mpi_comm = comm;
	
	

	MPI_Cart_shift( global_board->mpi_comm, 0, +1, &global_board->neighbourLeft, &global_board->neighbourRight);
	MPI_Cart_shift( global_board->mpi_comm, 1, +1, &global_board->neighbourUp, &global_board->neighbourDown);
	MPI_Comm_rank(comm, &global_board->mpi_rank);

	coords[0] = global_board->mpi_rankX - 1;
	coords[1] = global_board->mpi_rankY - 1;
	MPI_Cart_rank( comm, coords, &global_board->neighbourTL );

	coords[0] = global_board->mpi_rankX + 1;
	coords[1] = global_board->mpi_rankY - 1;
	MPI_Cart_rank( comm, coords, &global_board->neighbourTR );
	
	coords[0] = global_board->mpi_rankX - 1;
	coords[1] = global_board->mpi_rankY + 1;
	MPI_Cart_rank( comm, coords, &global_board->neighbourBL );
	
	coords[0] = global_board->mpi_rankX + 1;
	coords[1] = global_board->mpi_rankY + 1;
	MPI_Cart_rank( comm, coords, &global_board->neighbourBR );	
	
	global_board->sendBufLeft = malloc( GBOARD_LEFT_BUF_SIZE(*global_board->local_board) );
	global_board->sendBufRight = malloc( GBOARD_RIGHT_BUF_SIZE(*global_board->local_board)  );
	global_board->sendBufUp = malloc( GBOARD_UP_BUF_SIZE(*global_board->local_board) );
	global_board->sendBufDown = malloc( GBOARD_DOWN_BUF_SIZE(*global_board->local_board) );
	
	global_board->recvBufLeft = malloc( GBOARD_LEFT_BUF_SIZE(*global_board->local_board) );
	global_board->recvBufRight = malloc( GBOARD_RIGHT_BUF_SIZE(*global_board->local_board)  );
	global_board->recvBufUp = malloc( GBOARD_UP_BUF_SIZE(*global_board->local_board));
	global_board->recvBufDown = malloc( GBOARD_DOWN_BUF_SIZE(*global_board->local_board) );
	
	global_board->recvBufCourners = malloc( GBOARD_NUM_DIAG_DIRECTIONS * sizeof(field_t));
	
	assert(global_board);

	global_board->reqRecv = malloc( GBOARD_NUM_DIRECTIONS * sizeof(MPI_Request));
	global_board->reqSend = malloc( GBOARD_NUM_DIRECTIONS * sizeof(MPI_Request));
	
	
// 	printf("I'm process %i (%i,%i). left %i, right %i, up %i, down %i\n", global_board->mpi_rank, global_board->mpi_rankX, global_board->mpi_rankY,
// 		global_board->neighbourLeft, global_board->neighbourRight, global_board->neighbourUp, global_board->neighbourDown
// 	);

// 	

}



void globalBoard_print(globalBoard_t* board)
{
	
	board_t* bigLocalBoard = globalBoard_uniteLocalBoards(board);
	BEGIN_MASTER_ONLY_SECTION(*board)
		board_print(bigLocalBoard);
	END_MASTER_ONLY_SECTION
	board_destroy(bigLocalBoard);
	
}


void globalBoard_sendNeighbours( globalBoard_t* globalBoard )
{
	_logMaster(globalBoard,"Enter globalBoard_sendNeighbours");

// 	field_t* up = globalBoard->sendBufUp;
// 	field_t* down = globalBoard->sendBufDown;
// 	field_t* left = globalBoard->sendBufLeft;
// 	field_t* right = globalBoard->sendBufDown;
// 	
// 	board_t* board = globalBoard->local_board;
// 	
// 	field_t* origUp = up++;
// 	up += 2;
// 	field_t* origDown = down++;
// 	field_t* origLeft = left++;
// 	field_t* origRight = right++;
// 	right += 2;
// 	
// 	field_t tmp;
// 	
// 	for ( int x = 0; x < board->width; x++) {
// 		field_t cur = board->data[ BOARD_PADDING_X + x ];
// 		if(FIELD_WAS_CHANGED(cur)){
// 
// 			tmp.val = x;
// 			*up = tmp;
// 			up++;
// 			field_broadcastTop(&cur, up);
// 			up++;
// 		}
// 	}
// 
// 	
// 	for ( int x = 0; x < board->width; x++) {
// 		field_t cur = board->data[ BOARD_PADDING_X + x + (BOARD_PADDING_Y + board->height) * (board->width + 2 * BOARD_PADDING_X) ];
// 		if(FIELD_WAS_CHANGED(cur)){
// 
// 			tmp.val = x;
// 			*down = tmp;
// 			down++;
// 			field_broadcastBottom(&cur, down);
// 			down++;
// 		}
// 	}
// 
// 	
// 	for ( int y = 0; y < board->width; y++) {
// 		field_t curLeft = board->data[ (BOARD_PADDING_Y + board->height) * (y+1) ];
// 		field_t curRight = board->data[ (BOARD_PADDING_Y + board->height) * (y+2) - 1 ];
// 		if(FIELD_WAS_CHANGED(curLeft)){
// 
// 			tmp.val = y;
// 			*left = tmp;
// 			left++;
// 			field_broadcastLeft(&curLeft, left);
// 			left++;
// 		}
// 		
// 		if(FIELD_WAS_CHANGED(curRight)){
// 			
// 			tmp.val = y;
// 			*right = tmp;
// 			right++;
// 			field_broadcastRight(&curRight, right);
// 			right++;
// 		}
// 	}
// 
// 	
// 	tmp.val = (uint32_t) -1;
// 	origUp[1] = tmp; // will be recasted to int again
// 	field_broadcastTopLeft(board->data + 0, origUp + 2);
// 	origDown[1] = tmp;
// 	field_broadcastBottomLeft(&board->data[ (BOARD_PADDING_Y + board->height) * BOARD_LINE_SKIP(*board)], origDown + 2);
// 	
// 	tmp.val = board->width;
// 	*up = tmp; 
// 	up++;
// 	field_broadcastTopRight(&board->data[ BOARD_PADDING_X + board->width ], up);
// 	up++;
// 	*down = tmp; 
// 	down++;
// 	field_broadcastBottomRight(&board->data[ BOARD_PADDING_X + board->width + BOARD_LINE_SKIP(*board) * (board->height + BOARD_PADDING_Y) ], down);
// 	down++;
// 	
// 	
// 	
// 	tmp.val = left - origLeft;
// 	origLeft[0] = tmp;
// 	tmp.val = right - origRight;
// 	origRight[0] = tmp;
// 	tmp.val = up - origUp;
// 	origUp[0] = tmp;
// 	tmp.val = down - origDown;
// 	origDown[0] = tmp;
// 	
// 
// 
	
		

	board_t* lBoard = globalBoard->local_board;
	
	MPI_Isend(BOARD_GET_FIELD_PTR(lBoard, 0, -1), GBOARD_UP_BUF_ELEMENTS(*lBoard), MPI_INT, globalBoard->neighbourUp, DIRECTION_UP, globalBoard->mpi_comm, &globalBoard->reqSend[DIRECTION_UP]);
	
	MPI_Isend(BOARD_GET_FIELD_PTR(lBoard, 0, lBoard->heightDiv3), GBOARD_DOWN_BUF_ELEMENTS(*lBoard), MPI_INT, globalBoard->neighbourDown, DIRECTION_DOWN, globalBoard->mpi_comm, &globalBoard->reqSend[DIRECTION_DOWN]);
	
	for ( int y = 0; y < GBOARD_LEFT_BUF_ELEMENTS(*globalBoard->local_board); y++ ) {
		globalBoard->sendBufLeft[y].val = BOARD_GET_FIELD_PTR(lBoard, -1, y)->val;
		globalBoard->sendBufRight[y].val = BOARD_GET_FIELD_PTR(lBoard, lBoard->widthDiv4, y)->val;
	}
	
	MPI_Isend((void*) globalBoard->sendBufLeft, GBOARD_LEFT_BUF_ELEMENTS(*globalBoard->local_board), MPI_INT, globalBoard->neighbourLeft, DIRECTION_LEFT, globalBoard->mpi_comm, &globalBoard->reqSend[DIRECTION_LEFT]);
	
	MPI_Isend((void*) globalBoard->sendBufRight, GBOARD_RIGHT_BUF_ELEMENTS(*globalBoard->local_board), MPI_INT, globalBoard->neighbourRight, DIRECTION_RIGHT, globalBoard->mpi_comm, &globalBoard->reqSend[DIRECTION_RIGHT]);
	
	
	// diagonal directions
	
	MPI_Isend(BOARD_GET_FIELD_PTR(lBoard, -1, -1), 1, MPI_INT, globalBoard->neighbourTL, DIRECTION_TL, globalBoard->mpi_comm, &globalBoard->reqSend[DIRECTION_TL]);
	
	MPI_Isend(BOARD_GET_FIELD_PTR(lBoard, lBoard->widthDiv4, -1), 1, MPI_INT, globalBoard->neighbourTR, DIRECTION_TR, globalBoard->mpi_comm, &globalBoard->reqSend[DIRECTION_TR]);
	
	MPI_Isend(BOARD_GET_FIELD_PTR(lBoard, -1, lBoard->heightDiv3), 1, MPI_INT, globalBoard->neighbourBL, DIRECTION_BL, globalBoard->mpi_comm, &globalBoard->reqSend[DIRECTION_BL]);
	
	MPI_Isend(BOARD_GET_FIELD_PTR(lBoard, lBoard->widthDiv4, lBoard->heightDiv3), 1, MPI_INT, globalBoard->neighbourBR, DIRECTION_BR, globalBoard->mpi_comm, &globalBoard->reqSend[DIRECTION_BR]);

	
// 	MPI_Isend((void*) origLeft, GBOARD_LEFT_BUF_SIZE(*globalBoard->local_board), MPI_INT, globalBoard->neighbourLeft, 0, globalBoard->mpi_comm, globalBoard->reqSendLeft);
// 	
// 	MPI_Isend((void*) origRight, GBOARD_RIGHT_BUF_SIZE(*globalBoard->local_board), MPI_INT, globalBoard->neighbourRight, 0, globalBoard->mpi_comm, globalBoard->reqSendRight);
// 	
// 	MPI_Isend((void*) origUp, GBOARD_UP_BUF_SIZE(*globalBoard->local_board), MPI_INT, globalBoard->neighbourUp, 0, globalBoard->mpi_comm, globalBoard->reqSendUp);
// 	
// 	MPI_Isend((void*) origDown, GBOARD_DOWN_BUF_SIZE(*globalBoard->local_board), MPI_INT, globalBoard->neighbourDown, 0, globalBoard->mpi_comm, globalBoard->reqSendDown);
	
	_logMaster(globalBoard,"Exit globalBoard_sendNeighbours");
}

void globalBoard_recvNeighbours( globalBoard_t* board )
{
	_logMaster(board,"Enter globalBoard_recvNeighbours");
	


	MPI_Irecv((void*) board->recvBufUp, GBOARD_UP_BUF_ELEMENTS(*board->local_board), MPI_INT, board->neighbourUp, DIRECTION_DOWN, board->mpi_comm, &board->reqRecv[DIRECTION_DOWN]);
	MPI_Irecv((void*) board->recvBufDown, GBOARD_DOWN_BUF_ELEMENTS(*board->local_board), MPI_INT, board->neighbourDown, DIRECTION_UP, board->mpi_comm, &board->reqRecv[DIRECTION_UP]);
	MPI_Irecv((void*) board->recvBufLeft, GBOARD_LEFT_BUF_ELEMENTS(*board->local_board), MPI_INT, board->neighbourLeft, DIRECTION_RIGHT, board->mpi_comm, &board->reqRecv[DIRECTION_RIGHT]);
	MPI_Irecv((void*) board->recvBufRight, GBOARD_RIGHT_BUF_ELEMENTS(*board->local_board), MPI_INT, board->neighbourRight, DIRECTION_LEFT, board->mpi_comm, &board->reqRecv[DIRECTION_LEFT]);
	
	MPI_Irecv(&board->recvBufCourners[COURNER_TL], 1, MPI_INT, board->neighbourTL, DIRECTION_BR, board->mpi_comm, &board->reqRecv[DIRECTION_BR]);
	MPI_Irecv(&board->recvBufCourners[COURNER_TR], 1, MPI_INT, board->neighbourTR, DIRECTION_BL, board->mpi_comm, &board->reqRecv[DIRECTION_BL]);
	MPI_Irecv(&board->recvBufCourners[COURNER_BL], 1, MPI_INT, board->neighbourBL, DIRECTION_TR, board->mpi_comm, &board->reqRecv[DIRECTION_TR]);
	MPI_Irecv(&board->recvBufCourners[COURNER_BR], 1, MPI_INT, board->neighbourBR, DIRECTION_TL, board->mpi_comm, &board->reqRecv[DIRECTION_TL]);
	
}

void globalBoard_processRecv( globalBoard_t* board ) {
	_logMaster(board,"Enter globalBoard_processRecv");
	

	MPI_Waitall(GBOARD_NUM_PERPENDICULAR_DIRECTIONS, board->reqRecv, MPI_STATUS_IGNORE);
	MPI_Waitall(GBOARD_NUM_PERPENDICULAR_DIRECTIONS, board->reqSend, MPI_STATUS_IGNORE);
	
	board_t* b = board->local_board;
	for( int x = 0; x < GBOARD_UP_BUF_ELEMENTS(*b); x++ ) {
		if ( PERIODIC_BOUNDARY || board->mpi_rankY != 0 ) {
			b->data[x + BOARD_PADDING_X + BOARD_LINE_SKIP(*b) * BOARD_PADDING_Y].val &= 
				~(FIELD_ALL_NEIGHBOURS_TOP_MASK  | FIELD_ALL_NEIGHBOURS_TOP_RIGHT_MASK | FIELD_ALL_NEIGHBOURS_TOP_LEFT_MASK);
			b->data[x + BOARD_PADDING_X + BOARD_LINE_SKIP(*b) * BOARD_PADDING_Y].val |= board->recvBufUp[x].val ;
		}
		if ( PERIODIC_BOUNDARY || board->mpi_rankY != board->mpi_sizeY - 1 ){
			
			b->data[x + BOARD_PADDING_X + ( b->heightDiv3 ) * BOARD_LINE_SKIP(*b)].val &= ~(FIELD_ALL_NEIGHBOURS_BOTTOM_MASK | FIELD_ALL_NEIGHBOURS_BOTTOM_LEFT_MASK | FIELD_ALL_NEIGHBOURS_BOTTOM_RIGHT_MASK);
			b->data[x + BOARD_PADDING_X + ( b->heightDiv3 ) * BOARD_LINE_SKIP(*b)].val |= board->recvBufDown[x].val;
		}
	}
	
	for( int y = 0; y < GBOARD_LEFT_BUF_ELEMENTS(*board->local_board); y++ ) {
		if ( PERIODIC_BOUNDARY || board->mpi_rankX != 0 ) {
			
			board->local_board->data[(y + BOARD_PADDING_Y) * BOARD_LINE_SKIP(*board->local_board) + BOARD_PADDING_X].val &= 
				~(FIELD_ALL_NEIGHBOURS_LEFT_MASK | FIELD_ALL_NEIGHBOURS_TOP_LEFT_MASK | FIELD_ALL_NEIGHBOURS_BOTTOM_LEFT_MASK);
			board->local_board->data[(y + BOARD_PADDING_Y) * BOARD_LINE_SKIP(*board->local_board) + BOARD_PADDING_X].val |= board->recvBufLeft[y].val;
		}
		if ( PERIODIC_BOUNDARY || board->mpi_rankX != board->mpi_sizeX - 1) {	
			board->local_board->data[(y + BOARD_PADDING_Y) * BOARD_LINE_SKIP(*board->local_board) + board->local_board->widthDiv4].val &= ~(FIELD_ALL_NEIGHBOURS_RIGHT_MASK | FIELD_ALL_NEIGHBOURS_TOP_RIGHT_MASK | FIELD_ALL_NEIGHBOURS_BOTTOM_RIGHT_MASK);
			board->local_board->data[(y + BOARD_PADDING_Y) * BOARD_LINE_SKIP(*board->local_board) + board->local_board->widthDiv4].val |= board->recvBufRight[y].val;
		}
	}
	
	MPI_Waitall(GBOARD_NUM_PERPENDICULAR_DIRECTIONS, board->reqRecv + GBOARD_NUM_PERPENDICULAR_DIRECTIONS, MPI_STATUS_IGNORE);
	MPI_Waitall(GBOARD_NUM_PERPENDICULAR_DIRECTIONS, board->reqSend + GBOARD_NUM_PERPENDICULAR_DIRECTIONS, MPI_STATUS_IGNORE);

	if( PERIODIC_BOUNDARY ||  !(board->mpi_rankX == 0 || board->mpi_rankY == 0)) {
		BOARD_GET_FIELD_PTR(b, 0, 0)->val &= ~FIELD_ALL_NEIGHBOURS_TOP_LEFT_MASK; 
		BOARD_GET_FIELD_PTR(b, 0, 0)->val |= board->recvBufCourners[COURNER_TL].val;
	}

	if( PERIODIC_BOUNDARY || !(board->mpi_rankX == board->mpi_sizeX - 1 || board->mpi_rankY == 0)) {
		BOARD_GET_FIELD_PTR(b, b->widthDiv4 - 1, 0)->val &= ~FIELD_ALL_NEIGHBOURS_TOP_RIGHT_MASK; 
		BOARD_GET_FIELD_PTR(b, b->widthDiv4 - 1, 0)->val |= board->recvBufCourners[COURNER_TR].val;
	}
	
	if( PERIODIC_BOUNDARY || !(board->mpi_rankX == 0 || board->mpi_rankY == board->mpi_sizeY - 1)) {
		BOARD_GET_FIELD_PTR(b, 0, b->heightDiv3 - 1)->val &= ~FIELD_ALL_NEIGHBOURS_BOTTOM_LEFT_MASK; 
		BOARD_GET_FIELD_PTR(b, 0, b->heightDiv3- 1)->val |= board->recvBufCourners[COURNER_BL].val;
	}
	if( PERIODIC_BOUNDARY || !((board->mpi_rankX == board->mpi_sizeX - 1) || (board->mpi_rankY == board->mpi_sizeY - 1))) {
		BOARD_GET_FIELD_PTR(b, b->widthDiv4 - 1, b->heightDiv3 - 1)->val &= ~FIELD_ALL_NEIGHBOURS_BOTTOM_RIGHT_MASK; 
		BOARD_GET_FIELD_PTR(b, b->widthDiv4 - 1, b->heightDiv3 - 1)->val |= board->recvBufCourners[COURNER_BR].val;
	}
}


void globalBoard_step(globalBoard_t* board)
{
	_logMaster(board,"Enter globalBoard_step");
	


// 	board_updateFields(board->local_board);
// 	board_broadcastNeighbourhoods(board->local_board);
// 	
// 	globalBoard_sendNeighbours(board);
// 	globalBoard_recvNeighbours(board);	
// 	globalBoard_processRecv(board);
	
	
	board_broadcastNeighbourhoods(board->local_board);
	
	globalBoard_sendNeighbours(board);
	globalBoard_recvNeighbours(board);	
	board_updateInnerFields(board->local_board);
	globalBoard_processRecv(board);
	board_updateCornerFields(board->local_board);
	board_updateHorizontalMargins(board->local_board);
	board_updateVerticalMargins(board->local_board);
}



void globalBoard_destroy(globalBoard_t* board)
{
	board_destroy(board->local_board);
	
	free(board->recvBufDown);
	free(board->recvBufUp);
	free(board->recvBufLeft);
	free(board->recvBufRight);
	free(board->recvBufCourners);
	
	free(board->sendBufDown);
	free(board->sendBufUp);
	free(board->sendBufLeft);
	free(board->sendBufRight);
	
	free(board->reqRecv);
	free(board->reqSend);
	
	free(board);
}

board_t* globalBoard_uniteLocalBoards(globalBoard_t* board)
{
	
	_logMaster(board, "Enter globalBoard_uniteLocalBoards");
	
	
	board_t* mergedBoard = board_create(board->global_width, board->global_height);
	board_reset(mergedBoard);
	
	if( board->mpi_rank != 0) {
		for (int i = 0; i < board->local_board->heightDiv3; i++){
			MPI_Send((void*) (board->local_board->data + 1 + (i+1) * BOARD_LINE_SKIP(*board->local_board)), board->local_board->widthDiv4, MPI_INT, 0, 0, board->mpi_comm);
			
		}
	}
	
	BEGIN_MASTER_ONLY_SECTION(*board)
	
	for (int y = 0; y < board->local_board->heightDiv3; y++){
		int idx = (1) + (y + 1) * BOARD_LINE_SKIP(*mergedBoard);
		memcpy(&mergedBoard->data[idx], &board->local_board->data[ 1 + (y + 1) * BOARD_LINE_SKIP(*board->local_board)], sizeof(field_t) * board->local_board->widthDiv4);
	}
	
	for ( int srcRank = 1; srcRank < board->mpi_sizeX * board->mpi_sizeY; srcRank++) {
		
		for (int y = 0; y < board->local_board->heightDiv3; y++){
			int coords[2];
			MPI_Cart_coords(board->mpi_comm, srcRank, 2, coords);
			int idx = (1 + coords[0] * board->local_board->widthDiv4) + (1 + coords[1] * board->local_board->heightDiv3 + y) * BOARD_LINE_SKIP(*mergedBoard);
			MPI_Status stat;
			
 			MPI_Recv((void*) &mergedBoard->data[idx], board->local_board->widthDiv4, MPI_INT, srcRank,  0, board->mpi_comm, MPI_STATUS_IGNORE);
			
			
		}
	}
	END_MASTER_ONLY_SECTION

	MPI_Barrier(board->mpi_comm);
		
	return mergedBoard;
}

board_t* globalBoard_uniteLocalBoardsWithMargin(globalBoard_t* board)
{
	
	_logMaster(board, "Enter globalBoard_uniteLocalBoards");
	
	
	board_t* mergedBoard = board_create(board->global_width + 2*BACTERIA_PER_FIELD_X, board->global_height + 2*BACTERIA_PER_FIELD_Y);
	board_reset(mergedBoard);
	
	if( board->mpi_rank != 0) {
		for (int i = 0; i < board->local_board->heightDiv3 + 2* BACTERIA_PER_FIELD_Y; i++){
			MPI_Send((void*) (board->local_board->data + i * BOARD_LINE_SKIP(*board->local_board)), board->local_board->widthDiv4 + 2, MPI_INT, 0, 0, board->mpi_comm);
			
		}
	}
	
	BEGIN_MASTER_ONLY_SECTION(*board)
	
	for (int y = 0; y < board->local_board->heightDiv3; y++){
		int idx = (1) + (y + 1) * BOARD_LINE_SKIP(*mergedBoard);
		memcpy(&mergedBoard->data[idx], &board->local_board->data[ 1 + (y + 1) * BOARD_LINE_SKIP(*board->local_board)], sizeof(field_t) * board->local_board->widthDiv4);
	}
	
	for ( int srcRank = 1; srcRank < board->mpi_sizeX * board->mpi_sizeY; srcRank++) {
		
		for (int y = 0; y < board->local_board->heightDiv3 + 2; y++){
			int coords[2];
			MPI_Cart_coords(board->mpi_comm, srcRank, 2, coords);
			int idx = ( coords[0] * (board->local_board->widthDiv4 + 2)) + (coords[1] * (board->local_board->heightDiv3 + 2 )+ y) * BOARD_LINE_SKIP(*mergedBoard);
			MPI_Status stat;
			
 			MPI_Recv((void*) &mergedBoard->data[idx], board->local_board->widthDiv4 + 2, MPI_INT, srcRank,  0, board->mpi_comm, MPI_STATUS_IGNORE);
			
			
		}
	}
	END_MASTER_ONLY_SECTION

	MPI_Barrier(board->mpi_comm);
		
	return mergedBoard;
}


void globalBoard_printDebug(globalBoard_t* board)
{

	board_t* bigLocalBoard = globalBoard_uniteLocalBoards(board);
	BEGIN_MASTER_ONLY_SECTION(*board)
		board_printDebug(bigLocalBoard);
	END_MASTER_ONLY_SECTION
	board_destroy(bigLocalBoard);
}

void globalBoard_fillRandomly(globalBoard_t* board)
{
	_logMaster(board, "Enter globalBoard_fillRandomly");
	
	
	board_fillRandomly(board->local_board);


	
	MPI_Barrier(board->mpi_comm);
 	globalBoard_sendNeighbours(board);
 	globalBoard_recvNeighbours(board);
	globalBoard_processRecv(board);	
	MPI_Barrier(board->mpi_comm);
}


