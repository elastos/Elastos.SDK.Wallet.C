cmake_minimum_required(VERSION 3.10)

include(BuildType.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SYSTEM_NAME "${CMAKE_SYSTEM_NAME}")
set(SYSTEM_ARCH "${CMAKE_SYSTEM_PROCESSOR}")

if (NOT DEFINED BUILD_ROOT)
	set(BUILD_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/build/sysroot/${SYSTEM_NAME}/${SYSTEM_ARCH}")
endif()
find_package(PkgConfig REQUIRED)
set(ENV{PKG_CONFIG_PATH} "${BUILD_ROOT}/lib/pkgconfig")

include_directories(
  ${PROJECT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(src)

message(STATUS "****************************")
message(STATUS "SYSTEM_NAME : ${SYSTEM_NAME}")
message(STATUS "SYSTEM_ARCH : ${SYSTEM_ARCH}")
message(STATUS "BUILD_ROOT  : ${BUILD_ROOT}")
message(STATUS "****************************")
