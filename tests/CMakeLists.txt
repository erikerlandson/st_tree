# unit testing is built on the boost unit testing package
find_package(Boost 1.36 COMPONENTS unit_test_framework)

# include path to st_tree.h
include_directories(${st_tree_SOURCE_DIR}/include)

if (Boost_FOUND)
    add_executable(unit_tests
                   unit_tests.cpp
                   ut_exception.cpp
                   ut_vmap_iter.cpp
                   ut_raw.cpp
                   ut_ordered.cpp
                   ut_keyed.cpp
                  )
    include_directories(${Boost_INCLUDE_DIR})
    target_link_libraries(unit_tests ${Boost_LIBRARIES})
endif ()
