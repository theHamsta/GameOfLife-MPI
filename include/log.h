#ifndef LOG_H
#define LOG_H

#include <stdio.h>



#define _log(MESSAGE, ...) printf("DEBUG: " MESSAGE " [%s:%i]\n", ##__VA_ARGS__, __FILE__, __LINE__); fflush(stdout)
#define _error(MESSAGE, ...) fprintf(stderr, "ERROR: " MESSAGE " [%s:%i]\n", ##__VA_ARGS__, __FILE__, __LINE__)
#define _logMaster(BOARD,MESSAGE, ...) if(BOARD->mpi_rank==0) { printf("DEBUG: " MESSAGE " [%s:%i]\n", ##__VA_ARGS__, __FILE__, __LINE__); fflush(stdout);}

#endif // #ifndef LOG_H