cmake_minimum_required(VERSION 3.17)

project(OpenCmdAnywhere VERSION 1.1.2 LANGUAGES CXX)
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
include(FetchContent)

set(3RDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
include_directories(${3RDPARTY_DIR}/json)

FetchContent_Declare(
    easy_translate
    SOURCE_DIR ${3RDPARTY_DIR}/easy_translate
)
FetchContent_MakeAvailable(easy_translate)

FetchContent_Declare(
    global_hotkey
    SOURCE_DIR ${3RDPARTY_DIR}/global_hotkey
)
set(GLOBAL_HOTKEY_DISABLE_REGISTER ON)
FetchContent_MakeAvailable(global_hotkey)

# 重定向目标文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

# 设置程序信息
set(APP_ORGANIZATION "JaderoChan")
set(APP_ORGANIZATION_DOMAIN "io.github.jaderochan")
set(APP_TITLE ${PROJECT_NAME})
set(APP_VERSION ${PROJECT_VERSION})
set(APP_COPYRIGHT_TEXT "Copyright (c) 2025 JaderoChan")
set(APP_UUID "0d0e9c33-5780-4da4-8051-c95dcbcac075")

add_subdirectory(src)
