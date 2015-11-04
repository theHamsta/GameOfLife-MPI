#include "global_board.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "log.h"



#define GBOARD_UP_BUF_SIZE(BOARD) ( 2 * sizeof(field_t) * ((BOARD).width + 2 * BOARD_PADDING_X)) 
#define GBOARD_DOWN_BUF_SIZE(BOARD) GBOARD_UP_BUF_SIZE(BOARD)
#define GBOARD_LEFT_BUF_SIZE(BOARD) ( 2 * sizeof(field_t) * (BOARD).height)
#define GBOARD_RIGHT_BUF_SIZE(BOARD) GBOARD_LEFT_BUF_SIZE(BOARD)

#define BEGIN_MASTER_ONLY_SECTION(GLOBAL_BOARD) if ((GLOBAL_BOARD).mpi_rank == 0) {

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
	
	global_board->mpi_comm = comm;
	
	MPI_Cart_shift( global_board->mpi_comm, 0, -1, &global_board->mpi_rank, &global_board->neighbourLeft);
	MPI_Cart_shift( global_board->mpi_comm, 0, +1, &global_board->mpi_rank, &global_board->neighbourRight);
	MPI_Cart_shift( global_board->mpi_comm, 1, -1, &global_board->mpi_rank, &global_board->neighbourUp);
	MPI_Cart_shift( global_board->mpi_comm, 1, +1, &global_board->mpi_rank, &global_board->neighbourDown);
	MPI_Comm_rank(comm, &global_board->mpi_rank);

	
	global_board->sendBufLeft = malloc( GBOARD_LEFT_BUF_SIZE(*global_board->local_board) );
	global_board->sendBufRight = malloc( GBOARD_RIGHT_BUF_SIZE(*global_board->local_board)  );
	global_board->sendBufUp = malloc( GBOARD_UP_BUF_SIZE(*global_board->local_board) );
	global_board->sendBufDown = malloc( GBOARD_DOWN_BUF_SIZE(*global_board->local_board) );
	
	global_board->recvBufLeft = malloc( GBOARD_LEFT_BUF_SIZE(*global_board->local_board) );
	global_board->recvBufRight = malloc( GBOARD_RIGHT_BUF_SIZE(*global_board->local_board)  );
	global_board->recvBufUp = malloc( GBOARD_UP_BUF_SIZE(*global_board->local_board) );
	global_board->recvBufDown = malloc( GBOARD_DOWN_BUF_SIZE(*global_board->local_board) );
	
	assert(global_board);

	MPI_Request* reqPool = malloc( 8 * sizeof(MPI_Request));
	global_board->reqRecvUp = reqPool + 0;
	global_board->reqRecvDown = reqPool + 1;
	global_board->reqRecvLeft = reqPool + 2;
	global_board->reqRecvRight = reqPool + 3;
	global_board->reqSendUp = reqPool + 4;
	global_board->reqSendDown = reqPool + 5;
	global_board->reqSendLeft = reqPool + 6;
	global_board->reqSendRight = reqPool + 7;
	
}

void globalBoard_fillRandomly(globalBoard_t* board)
{
	_log("Enter globalBoard_fillRandomly");
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
	_log("Enter globalBoard_sendNeighbours");

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
	

	
		
	
	MPI_Isend((void*) origLeft, GBOARD_LEFT_BUF_SIZE(*globalBoard->local_board), MPI_INT, globalBoard->neighbourLeft, 0, globalBoard->mpi_comm, globalBoard->reqSendLeft);
	
	MPI_Isend((void*) origRight, GBOARD_RIGHT_BUF_SIZE(*globalBoard->local_board), MPI_INT, globalBoard->neighbourRight, 0, globalBoard->mpi_comm, globalBoard->reqSendRight);
	
	MPI_Isend((void*) origUp, GBOARD_UP_BUF_SIZE(*globalBoard->local_board), MPI_INT, globalBoard->neighbourUp, 0, globalBoard->mpi_comm, globalBoard->reqSendUp);
	
	MPI_Isend((void*) origDown, GBOARD_DOWN_BUF_SIZE(*globalBoard->local_board), MPI_INT, globalBoard->neighbourDown, 0, globalBoard->mpi_comm, globalBoard->reqSendDown);
	
	_log("Exit globalBoard_sendNeighbours");
}

void globalBoard_recvNeighbours( globalBoard_t* board )
{
	_log("Enter globalBoard_recvNeighbours");
	
	*board->reqRecvUp = 0;
	*board->reqRecvDown = 0;
	*board->reqRecvLeft = 0;
	*board->reqRecvRight = 0; 

	MPI_Irecv((void*) board->recvBufUp, GBOARD_UP_BUF_SIZE(*board->local_board), MPI_INT, board->neighbourDown, 0, board->mpi_comm, board->reqRecvUp);
	MPI_Irecv((void*) board->recvBufDown, GBOARD_DOWN_BUF_SIZE(*board->local_board), MPI_INT, board->neighbourUp, 0, board->mpi_comm, board->reqRecvDown);
	MPI_Irecv((void*) board->recvBufLeft, GBOARD_LEFT_BUF_SIZE(*board->local_board), MPI_INT, board->neighbourRight, 0, board->mpi_comm, board->reqRecvLeft);
	MPI_Irecv((void*) board->recvBufRight, GBOARD_RIGHT_BUF_SIZE(*board->local_board), MPI_INT, board->neighbourLeft, 0, board->mpi_comm, board->reqSendRight);
}

void globalBoard_processRecv( globalBoard_t* board ) {
	_log("Enter globalBoard_processRecv");
}


void globalBoard_step(globalBoard_t* board)
{
	_log("Enter globalBoard_step");
	
	globalBoard_sendNeighbours(board);
	globalBoard_recvNeighbours(board);
	board_broadcastNeighbourhoods(board->local_board);
	globalBoard_processRecv(board);
	board_updateFields(board->local_board);
	
}



void globalBoard_destroy(globalBoard_t* board)
{
	board_destroy(board->local_board);
	
	free(board->recvBufDown);
	free(board->recvBufUp);
	free(board->recvBufLeft);
	free(board->recvBufRight);
	
	free(board->sendBufDown);
	free(board->sendBufUp);
	free(board->sendBufLeft);
	free(board->sendBufRight);
	
	free(board->reqRecvUp);
	
	free(board);
}

board_t* globalBoard_uniteLocalBoards(globalBoard_t* board)
{
	
	_log("Enter globalBoard_uniteLocalBoards");
	
	
	board_t* mergedBoard = board_create(board->global_width, board->global_height);
	
	
	if( board->mpi_rank != 0) {
		for (int i = 0; i < board->local_board->height / BACTERIA_PER_FIELD_Y; i++){
			_log("%i sending %i", board->mpi_rank , i);
			MPI_Send((void*) (board->local_board->data + 1 + BOARD_LINE_SKIP(*board->local_board)), board->local_board->width / BACTERIA_PER_FIELD_X, MPI_INT, 0, 0, board->mpi_comm);
			
		}
	}
	
	BEGIN_MASTER_ONLY_SECTION(*board)
	
	for (int y = 0; y < board->local_board->height / BACTERIA_PER_FIELD_Y; y++){
		int idx = (1) + (y + 1) * BOARD_LINE_SKIP(*mergedBoard);
		memcpy(&mergedBoard->data[idx], &board->local_board->data[ 1 + (y + 1) * BOARD_LINE_SKIP(*board->local_board)], sizeof(field_t) * board->local_board->width / BACTERIA_PER_FIELD_X);
	}
	
	for ( int srcRank = 1; srcRank < board->mpi_sizeX * board->mpi_sizeY; srcRank++) {
		
		for (int y = 0; y < board->local_board->height / BACTERIA_PER_FIELD_Y; y++){
			int coords[2];
			MPI_Cart_coords(board->mpi_comm, srcRank, 2, coords);
			int idx = (1 + coords[0] * board->local_board->width / BACTERIA_PER_FIELD_X) + (1 + coords[1] * board->local_board->height / BACTERIA_PER_FIELD_Y + y) * BOARD_LINE_SKIP(*mergedBoard);
			MPI_Status stat;
			
 			MPI_Recv((void*) &mergedBoard->data[idx], board->local_board->width / BACTERIA_PER_FIELD_X, MPI_INT, srcRank,  0, board->mpi_comm, MPI_STATUS_IGNORE);
			
			
		}
	}
	END_MASTER_ONLY_SECTION

	MPI_Barrier(board->mpi_comm);
		
	return mergedBoard;
}



