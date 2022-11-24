cmake_minimum_required(VERSION 3.20)
project(FormalLanguages)

include(ExternalProject)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" ${CMAKE_SOURCE_DIR}/cmake)

set(EXTERNAL_INSTALL_LOCATION ${Testing_SOURCE_DIR}/3rdparty)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

set(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage --coverage")
set(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage --coverage")

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
)

FetchContent_MakeAvailable(googletest)
include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)

include(CodeCoverage)
add_custom_target(coverage_report)


file(GLOB_RECURSE INCLUDES_FILES src/*.hpp)
set(INCLUDES_DIRS "")
foreach(FILE ${INCLUDES_FILES})
    get_filename_component(DIR ${FILE} PATH)
    set(INCLUDES_DIRS ${INCLUDES_DIRS} ${DIR})
endforeach()
list(REMOVE_DUPLICATES INCLUDES_DIRS)

include_directories(${INCLUDES_DIRS})

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE TESTS tests/*.cpp)

add_executable(parser main.cpp ${SOURCES})
add_executable(test ${TESTS} ${SOURCES})

target_link_libraries(test gtest_main gtest)
setup_target_for_coverage(coverage . test coverage)
