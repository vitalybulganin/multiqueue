cmake_minimum_required(VERSION 3.5)
set(PROJECT_NAME gtest-multiqueue)
project(${PROJECT_NAME})

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

set(PROJECT_LIBS ${PROJECT_LIBS} multiqueue)
set(THREAD_LIBS ${THREAD_LIBS} pthread)
set(GTEST_LIBS ${GTEST_LIBS} gtest)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("..")
# Collecting a list of sources.
file(GLOB_RECURSE FILES_INC *.h)
file(GLOB_RECURSE FILES_SRC *.cpp)

add_executable(${PROJECT_NAME} ${FILES_INC} ${FILES_SRC})
target_link_libraries(
    ${PROJECT_NAME}
    ${PROJECT_LIBS}
    ${GTEST_LIBS}
    ${THREAD_LIBS}
)
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME} WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
