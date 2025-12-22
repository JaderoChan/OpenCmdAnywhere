cmake_minimum_required(VERSION 3.17)

project(OpenCmdAnywhere VERSION 1.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(OCAW_TITLE "Open CMD Anywhere")
set(OCAW_ORGANIZATION "JaderoChan")
set(OCAW_COPYRIGHT_TEXT "Copyright © 2025 JaderoChan")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/template/config.h.in
    ${CMAKE_BINARY_DIR}/include/config.h
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_SUFFIX "x64")
else()
    set(ARCH_SUFFIX "x86")
endif()

option(OCAW_OUTLOG "Whether output the log" OFF)
option(UPDATE_TRANSLATIONS_FILES "Whether update the tarnslations files" OFF)

set(3RDPARTY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
set(json_SOURCE_DIR ${3RDPARTY}/json)
set(easy_translate_SOURCE_DIR ${3RDPARTY}/easy_translate)
set(minilog_SOURCE_DIR ${3RDPARTY}/minilog)
message(STATUS "Start configure the Global Hotkey library...")
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchGlobalHotkey.cmake)
message(STATUS "Success to configure the Global Hotkey library.")

add_subdirectory(OpenCmdAnywhere)
