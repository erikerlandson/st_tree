include_directories(${ootree_SOURCE_DIR}/include)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi")

add_executable(x01_hello_world x01_hello_world.cpp)
add_executable(x02_ordered_children x02_ordered_children.cpp)
add_executable(x03_keyed_children x03_keyed_children.cpp)
add_executable(x04_sort_children x04_sort_children.cpp)
