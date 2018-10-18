cmake_minimum_required(VERSION 3.11)
project(avms C)

set(CMAKE_C_STANDARD 11)

execute_process(COMMAND uname -r OUTPUT_VARIABLE UNAMER)
string(CONCAT KPATH "/usr/src/linux-headers-" ${UNAMER}  "/include" )
string(REGEX REPLACE "\n" "" KPATH ${KPATH})
include_directories(BEFORE ${KPATH})
add_executable(avms
        virtual_touchpad.c virtual_touchpad.h)
