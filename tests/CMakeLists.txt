# unit testing is built on the boost unit testing package
find_package(Boost 1.36 COMPONENTS unit_test_framework)

# include path to st_tree.h
include_directories(${st_tree_SOURCE_DIR}/include)

if (Boost_FOUND)
    enable_testing()

    add_executable(unit_tests
                   unit_tests.cpp
                   ut_exception.cpp
                   ut_vmap_iter.cpp
                   ut_raw.cpp
                   ut_ordered.cpp
                   ut_keyed.cpp
                  )
    include_directories(${Boost_INCLUDE_DIR})
    target_compile_definitions(unit_tests PRIVATE BOOST_ALL_NO_LIB=1)
    target_link_libraries(unit_tests ${Boost_LIBRARIES})

    add_test(NAME unit_tests COMMAND unit_tests)
    add_custom_target(test COMMAND unit_tests DEPENDS unit_tests)
endif ()
