cmake_minimum_required(VERSION 3.17)

project(OpenCmdAnywhere VERSION 1.0.7 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(OCAW_TITLE "Open CMD Anywhere")
set(OCAW_ORGANIZATION "JaderoChan")
set(OCAW_COPYRIGHT_TEXT "Copyright © 2025 JaderoChan")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_SUFFIX "x64")
else()
    set(ARCH_SUFFIX "x86")
endif()

set(3RDPARTY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
set(json_SOURCE_DIR ${3RDPARTY}/json)
set(easy_translate_SOURCE_DIR ${3RDPARTY}/easy_translate)
set(minilog_SOURCE_DIR ${3RDPARTY}/minilog)
message(STATUS "Start configure the Global Hotkey library...")
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchGlobalHotkey.cmake)
message(STATUS "Success to configure the Global Hotkey library.")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

add_subdirectory(src)
