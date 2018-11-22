CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(VirtualTouchPadDriver)
SET(CMAKE_C_STANDARD 90)

add_executable(${PROJECT_NAME} virtual_touchpad.c)
target_include_directories(${PROJECT_NAME} PRIVATE "/usr/src/linux-source-4.16/include/")
