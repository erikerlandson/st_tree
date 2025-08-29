# unit testing is built on the boost unit testing package
find_package(Boost 1.70 COMPONENTS unit_test_framework REQUIRED)

# include path to st_tree.h
include_directories(${st_tree_SOURCE_DIR}/include)

enable_testing()

add_executable(unit_tests
    unit_tests.cpp
    ut_exception.cpp
    ut_vmap_iter.cpp
    ut_raw.cpp
    ut_ordered.cpp
    ut_keyed.cpp
)

target_compile_definitions(unit_tests PRIVATE BOOST_ALL_NO_LIB=1)
if (NOT Boost_USE_STATIC_LIBS)
    target_compile_definitions(unit_tests PRIVATE BOOST_TEST_DYN_LINK=1)
endif ()
target_link_libraries(unit_tests Boost::unit_test_framework)

add_test(NAME unit_tests COMMAND unit_tests)
add_custom_target(test DEPENDS unit_tests COMMAND ./unit_tests)

find_program(VALGRIND valgrind)
if (VALGRIND)
    message(STATUS "Valgrind found: ${VALGRIND}: adding memtest target")
    add_custom_target(memtest DEPENDS unit_tests COMMAND ${VALGRIND} --tool=memcheck --leak-check=full ./unit_tests)
endif ()