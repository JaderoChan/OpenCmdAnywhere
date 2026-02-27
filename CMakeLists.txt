cmake_minimum_required(VERSION 3.17)

project(OpenCmdAnywhere VERSION 1.3.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 默认使用Release构建
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 查找Qt库
find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

###############
# 配置第三方库 #
###############

include(FetchContent)
set(3RDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

# 配置Easy Translate库
FetchContent_Declare(
    easy_translate
    SOURCE_DIR ${3RDPARTY_DIR}/easy_translate
)
FetchContent_MakeAvailable(easy_translate)

# 配置Global Hotkey库
FetchContent_Declare(
    global_hotkey
    SOURCE_DIR ${3RDPARTY_DIR}/global_hotkey
)
if(WIN32 OR APPLE)
    set(GLOBAL_HOTKEY_DISABLE_HOOK ON)
else()
    set(GLOBAL_HOTKEY_DISABLE_REGISTER ON)
endif()
FetchContent_MakeAvailable(global_hotkey)

###############
# 设置程序信息 #
###############

set(APP_ORGANIZATION "JaderoChan")
set(APP_ORGANIZATION_DOMAIN "io.github.jaderochan")
set(APP_TITLE ${PROJECT_NAME})
set(APP_VERSION ${PROJECT_VERSION})
set(APP_COPYRIGHT_TEXT "Copyright (c) 2025 JaderoChan")
set(APP_UUID "273c5512-801d-4be2-9520-84bdd3a00c43")

###############
# 定义常用路径 #
###############

# CMake文件夹
set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# CMake模板文件夹
set(CMAKE_TEMPLATE_DIR ${CMAKE_DIR}/template)
# 语言文件夹路径
set(LANGUAGES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/languages)
# 资源文件夹路径
set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
# 应用图标文件夹路径
set(APP_ICONS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/icons)

# 重定向目标文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

#############
# 添加子目录 #
#############

add_subdirectory(src)
