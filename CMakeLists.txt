cmake_minimum_required(VERSION 3.17)

project(OpenCmdAnywhere VERSION 1.1.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 默认使用Release构建
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 查找Qt库
find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# 配置第三方库
set(3RDPARTY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
set(json_SOURCE_DIR ${3RDPARTY}/json)
set(easy_translate_SOURCE_DIR ${3RDPARTY}/easy_translate)
message(STATUS "Start configure the Global Hotkey library...")
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchGlobalHotkey.cmake)
message(STATUS "Configure the Global Hotkey library successfully.")

# 重定向目标文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

# 设置程序信息
set(APP_ORGANIZATION "JaderoChan")
set(APP_ORGANIZATION_DOMAIN "io.github.jaderochan")
set(APP_TITLE ${PROJECT_NAME})
set(APP_VERSION ${PROJECT_VERSION})
set(APP_COPYRIGHT_TEXT "Copyright © 2025 JaderoChan")
set(APP_UUID "579b0006-3c59-40cf-8342-0cdc3086daee")

add_subdirectory(src)
