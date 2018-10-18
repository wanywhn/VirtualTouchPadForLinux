cmake_minimum_required(VERSION 3.11)
project(VirtualTouchPadServer)

set(CMAKE_CXX_STANDARD 14)

add_definitions(" -Wall")
SET(CMAKE_CXX_FLAGS_DEBUG " -g -O0")
SET(CMAKE_CXX_FLAGS_RELEASE " -O3")

AUX_SOURCE_DIRECTORY( ./Src DIR_SRCS)
AUX_SOURCE_DIRECTORY( ./Include DIR_INCLUDES)

include_directories(Include)

add_executable(VirtualTouchPadServer
        ${DIR_INCLUDES}
        ${DIR_SRCS}
        )
target_link_libraries(VirtualTouchPadServer pthread)