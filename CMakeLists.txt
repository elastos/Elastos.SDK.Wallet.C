cmake_minimum_required(VERSION 3.0)

include(BuildType.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

if("$ENV{TARGET_PLATFORM}" STREQUAL "android")

    set(CMAKE_SYSTEM_NAME Android)
    set(CMAKE_ANDROID_NDK $ENV{ANDROID_NDK})
    set(CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION clang)

    string(APPEND CMAKE_C_FLAGS " -fPIC")

    #choose Android version. This is not the version of Cmake.
    if("$ENV{TARGET_ABI}" STREQUAL "arm64-v8a")
        set(CMAKE_SYSTEM_VERSION 21)
    else()
        set(CMAKE_SYSTEM_VERSION 19)
    endif("$ENV{TARGET_ABI}" STREQUAL "arm64-v8a")

    #choose Android ABI. Mine is armeabi-v7a
    message($ENV{TARGET_ABI})
    set(CMAKE_ANDROID_ARCH_ABI $ENV{TARGET_ABI})

    string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,--no-undefined")
elseif("$ENV{TARGET_PLATFORM}" STREQUAL "ios")
    set(CMAKE_SYSTEM_NAME Darwin)
    set(CMAKE_CROSSCOMPILING TRUE)

    set(CMAKE_OSX_SYSROOT $ENV{OSX_SDK})
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,-undefined,error")
else("$ENV{TARGET_PLATFORM}" STREQUAL "android")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,--no-undefined")
endif("$ENV{TARGET_PLATFORM}" STREQUAL "android")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " -s")
endif()

project(sdk.wallet)

set(CMAKE_VERBOSE_MAKEFILE OFF)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR})

#add_subdirectory(thirdparty)
#add_subdirectory(src)
