CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(VirtualTouchPadDriver)
SET(CMAKE_C_STANDARD 90)

add_executable(${PROJECT_NAME} virtual_touchpad.c)
target_include_directories(${PROJECT_NAME} PRIVATE "/usr/src/linux-headers-5.10.18-amd64-desktop/include/")
target_include_directories(${PROJECT_NAME} PRIVATE "/usr/src/linux-headers-5.10.18-amd64-desktop/arch/x86/include/")