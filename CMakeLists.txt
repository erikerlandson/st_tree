cmake_minimum_required(VERSION 3.0)
project(st_tree VERSION 1.0.6)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Build against ANSI c++ standards
IF (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC OR MINGW)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi -pedantic -std=c++11")
ENDIF()

# include
add_subdirectory(include)

# tutorial examples
add_subdirectory(examples)

# testing programs
add_subdirectory(tests)

set(ST_TREE_HEADERS
    include/st_tree_detail.h
    include/st_tree.h
    include/st_tree_iterators.h
    include/st_tree_nodes.h)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets)

# Makes the project importable from the build directory
export(EXPORT ${PROJECT_NAME}-targets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

install(FILES ${ST_TREE_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_package_config_file(cmake/${PROJECT_NAME}Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${${PROJECT_NAME}_VERSION}
    COMPATIBILITY AnyNewerVersion)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

install(EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}Targets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)
