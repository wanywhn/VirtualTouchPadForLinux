cmake_minimum_required(VERSION 3.7)
PROJECT (vtp-server)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)

add_definitions(" -Wall")
SET(CMAKE_CXX_FLAGS_DEBUG " -g -O0")
SET(CMAKE_CXX_FLAGS_RELEASE " -O3")
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

AUX_SOURCE_DIRECTORY(src DIR_SRCS)
AUX_SOURCE_DIRECTORY( ./Include DIR_INCLUDES)

include_directories(Include)

add_executable(${PROJECT_NAME}
        ${DIR_INCLUDES}
        ${DIR_SRCS}
        )
target_link_libraries(${PROJECT_NAME} pthread)

SET(_VERSION_MAJOR 1)
SET(_VERSION_MINOR 1)
SET(_VERSION_PATCH 0)

SET(CPACK_GENERATOR "DEB")
#设置版本信息
set(CPACK_PACKAGE_VERSION_MAJOR "${_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${_VERSION_PATCH}")

#设置安装包的包名，打好的包将会是packagename-version-linux.deb，如果不设置，默认是工程名
#set(CPACK_PACKAGE_NAME ${PROJET_NAME})

#设置程序名，就是程序安装后的名字
set(CPACK_DEBIAN_PACKAGE_NAME ${PROJET_NAME})

#设置架构
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

#设置依赖
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")

#设置section
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")

#设置priority
set(CPACK_DEBIAN_PACKAGE_PRIORITY "Optional")

#设置description
set(CPACK_PACKAGE_DESCRIPTION "虚拟触控板的服务端")

#设置联系方式
set(CPACK_PACKAGE_CONTACT "wanywhn@qq.com")

#设置维护人
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "wanywhn@qq.com")

SET(INSSCRIPT ${CMAKE_SOURCE_DIR}/INSSCRIPT)
#设置控制脚本，这些都是大包者本人自己准备的，没有就算了
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
	"${INSSCRIPT}/postinst"
	"${INSSCRIPT}/prerm")


INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
INSTALL(FILES virtual_touchpad_server.conf.example DESTINATION /etc
	RENAME virtual_touchpad_server.conf)
INSTALL(PROGRAMS Initscripts/vtp-server DESTINATION /etc/init.d)
INCLUDE(CPack)
