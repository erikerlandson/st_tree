cmake_minimum_required(VERSION 2.6)
project(st_tree)

# Build against ANSI c++ standards
IF (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC OR MINGW)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi -pedantic")
ENDIF()

# include
add_subdirectory(include)

# tutorial examples
add_subdirectory(examples)

# testing programs
add_subdirectory(tests)
