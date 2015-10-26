project(gameoflifempi)
cmake_minimum_required(VERSION 2.8)


add_executable(gameoflifempi main.cpp)

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})


target_link_libraries(gameoflifempi ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
  set_target_properties(gameoflifempi PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS} -std=c99")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(gameoflifempi PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()


add_subdirectory(include)
add_subdirectory(src)