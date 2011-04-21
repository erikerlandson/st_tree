cmake_minimum_required(VERSION 2.6)
project(ootree)

# Build against ANSI c++ standards
# At this point, I'm assuming gcc, or gcc-compatible flags
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi")

# include
add_subdirectory(include)

# tutorial examples
add_subdirectory(examples)

# testing programs
add_subdirectory(test)
