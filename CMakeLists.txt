cmake_minimum_required(VERSION 3.2)
project(ArcaneCollate)

set(WRITE_SRC
    src/cpp/arcanecollate/write/Collater.cpp
)

set(TESTS_SUITES
    tests/cpp/TestsMain.cpp

    tests/cpp/write/Collater_TestSuite.cpp
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/linux_x86)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/linux_x86)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/linux_x86)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++0x -Wall -Wno-varargs -fPIC")

include_directories(
    ${INCLUDE_DIRECTORIES}
    ${CMAKE_BINARY_DIR}/ext/ArcaneCore/include/
    src/cpp
    tests/cpp
)

link_directories(
    ${LINK_DIRECTORIES}
    ${CMAKE_BINARY_DIR}/ext/ArcaneCore/lib/linux_x86
    ${CMAKE_BINARY_DIR}/build/linux_x86
)

add_library(arcanecollate_write STATIC ${WRITE_SRC})

target_link_libraries(arcanecollate_write
    arcanecore_base
    arcanecore_io
)

add_executable(tests ${TESTS_SUITES})

target_link_libraries(tests
    arcanecore_test
    arcanecore_base
    arcanecore_io
    arcanecollate_write
)
