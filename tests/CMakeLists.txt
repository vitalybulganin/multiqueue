cmake_minimum_required(VERSION 3.1.0)
set(PROJECT_NAME gtest-mmt-grpc)
project(${PROJECT_NAME})
#
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND CMAKE_MODULE_PATH "/usr/local/lib/cmake/baalbek")

    include(/usr/local/lib/cmake/baalbek/FindBaalapi.cmake)
    include(/usr/local/lib/cmake/baalbek/FindBaalbek.cmake)
    include(/usr/local/lib/cmake/baalbek/FindNarklog.cmake)
    include(/usr/local/lib/cmake/baalbek/FindMaillog.cmake)

    set(MMTGRPC_LIBRARIES mmt-grpc)
endif ()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

set(CONTRIBUTE_LIBS mtc morphrus moonycode blackhole)
set(GRPC_LIBS ${GRPC_LIBS} grpc++ grpc++_unsecure grpc gpr protobuf ${GRPC_GRPCPP_REFLECTION_LIBRARY})
set(SSL_LIBS ${SSL_LIBS} ssl crypto z)
set(CURL_LIBS ${CURL_LIBS} curl)
set(THREAD_LIBS ${THREAD_LIBS} pthread)
set(GTEST_LIBS ${GTEST_LIBS} gtest)
# set(PROJECT_NAME_FINAL ${PROJECT_NAME}-${PROJECT_VERSION})
# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("../baal-stubs")
include_directories("../..")
# Collecting a list of sources.
file(GLOB_RECURSE FILES_INC *.h)
file(GLOB_RECURSE FILES_SRC *.cpp)
# Removing stubs source.
execute_process(COMMAND cp -rvf ${PROJECT_SOURCE_DIR}/pb ${PROJECT_BINARY_DIR}/pb)

add_executable(${PROJECT_NAME} ${FILES_INC} ${FILES_SRC})
target_link_libraries(
    ${PROJECT_NAME}
    ${PROJECT_LIBS}
    ${MMTGRPC_LIBRARIES}
    ${MAILLOG_LIBRARIES}
    ${NARKLOG_LIBRARIES}
    ${TOOLBEK_LIBRARIES}
    ${BAALAPI_LIBRARIES}
    ${CONTRIBUTE_LIBS}
    ${GRPC_LIBS}
    ${GTEST_LIBS}
    ${SSL_LIBS}
    ${CURL_LIBS}
    ${THREAD_LIBS}
)
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME} WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
