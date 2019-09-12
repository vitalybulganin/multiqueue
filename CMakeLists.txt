cmake_minimum_required(VERSION 3.5)
set(PROJECT_NAME multiqueue)
project(${PROJECT_NAME})

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

#if (BUILD_TESTING)
    # Building unit tests.
    add_subdirectory(tests)
#endif()

file(GLOB SOURCE_INC *.h *.hpp)
file(GLOB_RECURSE SOURCE_FILES *.h *.cpp)

add_library(${PROJECT_NAME} ${SOURCE_INC} ${SOURCE_FILES})

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/local/lib/multiqueue)
install(FILES ${SOURCE_INC} DESTINATION /usr/local/include/multiqueue)
