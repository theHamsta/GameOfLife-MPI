#include "global_board.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>




// typedef struct globalBoard_s
// {
// 	unsigned int global_width;
// 	unsigned int global_height;
// 	
// 	board_t* local_board;
// 	unsigned int local_x;
// 	unsigned int local_y;
// 	
// 	unsigned int mpi_rankX;
// 	unsigned int mpi_rankY;
// 	unsigned int mpi_sizeX;
// 	unsigned int mpi_sizeY;
// 	
// 	unsigned int neighbourLeft;
// 	unsigned int neighbourRight;
// 	unsigned int neighbourUp;
// 	unsigned int neighbourDown;
// 	
// 	MPI_Comm mpi_comm;
// 	
// 	field_t* bufLeft;
// 	field_t* bufRight;
// 	field_t* bufUp;
// 	field_t* bufDown;
// 	
// } globalBoard_t;


#define GBOARD_UP_BUF_SIZE(BOARD) ( 2 * sizeof(field_t) * ((BOARD).width + 2 * BOARD_PADDING_X)) 
#define GBOARD_DOWN_BUF_SIZE(BOARD) GBOARD_UP_BUF_SIZE(BOARD)
#define GBOARD_LEFT_BUF_SIZE(BOARD) ( 2 * sizeof(field_t) * (BOARD).height)
#define GBOARD_RIGHT_BUF_SIZE(BOARD) GBOARD_LEFT_BUF_SIZE(BOARD)

#define BEGIN_MASTER_ONLY_SECTION(GLOBAL_BOARD) if (((GLOBAL_BOARD).mpi_rankX == 0) && ((GLOBAL_BOARD).mpi_rankY == 0)) {

#define END_MASTER_ONLY_SECTION }

void field_broadcastLeft( field_t* field, field_t* neighbour );
void field_broadcastTopLeft( field_t* field, field_t* neighbour );
void field_broadcastTop( field_t* field, field_t* neighbour );
void field_broadcastTopRight( field_t* field, field_t* neighbour );
void field_broadcastRight( field_t* field, field_t* neighbour );
void field_broadcastBottomRight( field_t* field, field_t* neighbour );
void field_broadcastBottom( field_t* field, field_t* neighbour );
void field_broadcastBottomLeft( field_t* field, field_t* neighbour );


globalBoard_t* globalBoard_create(unsigned int width, unsigned int height, int rank, int numRanksX, int numRanksY )
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
	
	MPI_Barrier(MPI_COMM_WORLD);
	printf("Hello world from processor %s, rank(%i,%i) in Cartesian coordinates\n", processor_name, coords[0], coords[1]);
	fflush(stdout);
	MPI_Barrier(MPI_COMM_WORLD);
	
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
	
	
	
	global_board->neighbourLeft = (coords[0] + numRanksX - 1) % numRanksX;
	global_board->neighbourRight = (coords[0] + numRanksX + 1) % numRanksX;
	global_board->neighbourDown = (coords[1] + numRanksY - 1) % numRanksY;
	global_board->neighbourUp = (coords[1] + numRanksY + 1) % numRanksY;
	
	global_board->mpi_comm = comm;
	
	global_board->sendBufUp = malloc( 2 * sizeof(field_t) * global_board->global_height );
	global_board->sendBufRight = malloc( 2 * sizeof(field_t) * global_board->global_height );
	global_board->sendBufUp = malloc( 2 * sizeof(field_t) * (global_board->global_width + 2 * BOARD_PADDING_X ));
	global_board->sendBufDown = malloc( 2 * sizeof(field_t) * (global_board->global_width + 2 * BOARD_PADDING_X ));
	
	global_board->recvBufUp = malloc( 2 * sizeof(field_t) * global_board->global_height );
	global_board->recvBufRight = malloc( 2 * sizeof(field_t) * global_board->global_height );
	global_board->recvBufUp = malloc( 2 * sizeof(field_t) * (global_board->global_width + 2 * BOARD_PADDING_X ));
	global_board->recvBufDown = malloc( 2 * sizeof(field_t) * (global_board->global_width + 2 * BOARD_PADDING_X ));
	
	assert(global_board);

	
}

void globalBoard_fillRandomly(globalBoard_t* board)
{
	MPI_Barrier(board->mpi_comm);
	for ( int y = 0; y < board->mpi_sizeY; y++ ) {
		for ( int x = 0; x < board->mpi_sizeX; x++ ) {
			if ( board->mpi_rankX == x && board->mpi_rankY == y ) {
				board_fillRandomly(board->local_board);
			}
			MPI_Barrier(board->mpi_comm);
		}
	}
}


void globalBoard_print(globalBoard_t* board)
{
	
	MPI_Barrier(board->mpi_comm);
	
	
	for ( int y = 0; y < board->mpi_sizeY; y++ ) {
		for ( int x = 0; x < board->mpi_sizeX; x++ ) {
			if ( board->mpi_rankX == x && board->mpi_rankY == y ) {
				
				board_print(board->local_board);
				fflush(stdout);
				usleep(1000000);
			}
			MPI_Barrier(MPI_COMM_WORLD);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		BEGIN_MASTER_ONLY_SECTION(*board)
			printf("===================\n");
			fflush(stdout);
		END_MASTER_ONLY_SECTION
	}
}

void globalBoard_sendNeighbours( globalBoard_t* globalBoard )
{
	field_t* up = globalBoard->sendBufUp;
	field_t* down = globalBoard->sendBufDown;
	field_t* left = globalBoard->sendBufLeft;
	field_t* right = globalBoard->sendBufDown;
	
	board_t* board = globalBoard->local_board;
	
	field_t* origUp = up++;
	up += 2;
	field_t* origDown = down++;
	field_t* origLeft = left++;
	field_t* origRight = right++;
	right += 2;
	
	field_t tmp;
	
	for ( int x = 0; x < board->width; x++) {
		field_t cur = board->data[ BOARD_PADDING_X + x ];
		if(FIELD_WAS_CHANGED(cur)){

			tmp.val = x;
			*up = tmp;
			up++;
			field_broadcastTop(&cur, up);
			up++;
		}
	}
	
	for ( int x = 0; x < board->width; x++) {
		field_t cur = board->data[ BOARD_PADDING_X + x + (BOARD_PADDING_Y + board->height) * (board->width + 2 * BOARD_PADDING_X) ];
		if(FIELD_WAS_CHANGED(cur)){

			tmp.val = x;
			*down = tmp;
			down++;
			field_broadcastBottom(&cur, down);
			down++;
		}
	}

	
	for ( int y = 0; y < board->width; y++) {
		field_t curLeft = board->data[ (BOARD_PADDING_Y + board->height) * (y+1) ];
		field_t curRight = board->data[ (BOARD_PADDING_Y + board->height) * (y+2) - 1 ];
		if(FIELD_WAS_CHANGED(curLeft)){

			tmp.val = y;
			*left = tmp;
			left++;
			field_broadcastLeft(&curLeft, left);
			left++;
		}
		
		if(FIELD_WAS_CHANGED(curRight)){
			
			tmp.val = y;
			*right = tmp;
			right++;
			field_broadcastRight(&curRight, right);
			right++;
		}
	}

	tmp.val = (uint32_t) -1;
	origUp[1] = tmp; // will be recasted to int again
	field_broadcastTopLeft(board->data + 0, origUp + 2);
	origDown[1] = tmp;
	field_broadcastBottomLeft(&board->data[ (BOARD_PADDING_Y + board->height) * BOARD_LINE_SKIP(*board)], origDown + 2);
	
	tmp.val = board->width;
	*up = tmp; 
	up++;
	field_broadcastTopRight(&board->data[ BOARD_PADDING_X + board->width ], up);
	up++;
	*down = tmp; 
	down++;
	field_broadcastBottomRight(&board->data[ BOARD_PADDING_X + board->width + BOARD_LINE_SKIP(*board) * (board->height + BOARD_PADDING_Y) ], down);
	down++;
	
	tmp.val = left - origLeft;
	origLeft[0] = tmp;
	tmp.val = right - origRight;
	origRight[0] = tmp;
	tmp.val = up - origUp;
	origUp[0] = tmp;
	tmp.val = down - origDown;
	origDown[0] = tmp;
	
	
	MPI_Isend((void*) origLeft, 2 * sizeof(field_t) * (board->width + 2 * BOARD_PADDING_X), MPI_INT, globalBoard->neighbourLeft, 0, globalBoard->mpi_comm, globalBoard->reqSendLeft);
	
	MPI_Isend((void*) origRight, 2 * sizeof(field_t) * (board->width + 2 * BOARD_PADDING_X), MPI_INT, globalBoard->neighbourRight, 0, globalBoard->mpi_comm, globalBoard->reqSendRight);
	
	MPI_Isend((void*) origUp, 2 * sizeof(field_t) * board->height, MPI_INT, globalBoard->neighbourUp, 0, globalBoard->mpi_comm, globalBoard->reqSendUp);
	
	MPI_Isend((void*) origDown, 2 * sizeof(field_t) * board->width, MPI_INT, globalBoard->neighbourDown, 0, globalBoard->mpi_comm, globalBoard->reqSendDown);
}

void globalBoard_recvNeighbours( globalBoard_t* board )
{
	MPI_Irecv((void*) board->recvBufUp, GBOARD_UP_BUF_SIZE(*board->local_board), MPI_INT, board->neighbourDown, 0, board->mpi_comm, board->recvUp);
	MPI_Irecv((void*) board->recvBufDown, GBOARD_DOWN_BUF_SIZE(*board->local_board), MPI_INT, board->neighbourUp, 0, board->mpi_comm, board->recvDown);
	MPI_Irecv((void*) board->recvBufLeft, GBOARD_LEFT_BUF_SIZE(*board->local_board), MPI_INT, board->neighbourRight, 0, board->mpi_comm, board->recvLeft);
	MPI_Irecv((void*) board->recvBufRight, GBOARD_RIGHT_BUF_SIZE(*board->local_board), MPI_INT, board->neighbourLeft, 0, board->mpi_comm, board->recvRight);
}

void globalBoard_processRecv( globalBoard_t* board ) {
	
}


void globalBoard_step(globalBoard_t* board)
{
	globalBoard_sendNeighbours(board);
	globalBoard_recvNeighbours(board);
	board_broadcastNeighbourhoods(board->local_board);
	globalBoard_processRecv(board);
	board_updateFields(board->local_board);
	
}



void globalBoard_destroy(globalBoard_t* board)
{
	board_destroy(board->local_board);
	free(board);
}


