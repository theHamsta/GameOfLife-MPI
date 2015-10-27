project(gameoflifempi)
cmake_minimum_required(VERSION 2.8)


add_executable(gameoflifempi
				src/board.c
				src/global_board.c
				src/field.c
				main.cpp)

find_package(MPI REQUIRED)

include_directories(include ${MPI_INCLUDE_PATH})
set(CMAKE_C_FLAGS "-std=c99")

target_link_libraries(gameoflifempi ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
  set_target_properties(gameoflifempi PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(gameoflifempi PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

