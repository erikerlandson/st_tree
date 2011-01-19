# unit testing is built on the boost unit testing package
find_package(Boost 1.36 COMPONENTS unit_test_framework)

# include path to ootree.h
include_directories(${ootree_SOURCE_DIR}/include)

if (Boost_FOUND)
    add_executable(ootree_unit_test 
                   ootree_unit_test.cpp
                   ut_vmap_iter.cpp
                   ut_raw.cpp
                   ut_ordered.cpp
                   ut_keyed.cpp
                  )
    include_directories(${Boost_INCLUDE_DIR})
    target_link_libraries(ootree_unit_test ${Boost_LIBRARIES})
endif (Boost_FOUND)
