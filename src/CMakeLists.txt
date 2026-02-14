cmake_minimum_required(VERSION 3.17)

#############
# 收集源文件 #
#############

file(GLOB HEADER ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
if(WIN32)
    file(GLOB WIN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/platforms/*_win.cpp)
    list(APPEND SRC ${WIN_SRC})
elseif(APPLE)
    file(GLOB MAC_SRC ${CMAKE_CURRENT_SOURCE_DIR}/platforms/*_mac.cpp)
    list(APPEND SRC ${MAC_SRC})
endif()
file(GLOB UI ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)
file(GLOB QRC ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)
set(PROJECT_SOURCE ${HEADER} ${SRC} ${UI} ${QRC})

###########
# 构建程序 #
###########

qt_standard_project_setup()
qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCE})

###########
# 配置依赖 #
###########

configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/template/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/config.h
)

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${json_SOURCE_DIR}/include
    ${easy_translate_SOURCE_DIR}/include
)

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    global_hotkey::global_hotkey
    Qt${QT_VERSION_MAJOR}::Widgets
)

###############
# 配置生成目标 #
###############

set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

if(WIN32)
    set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON
    )
endif()

option(UPDATE_TRANSLATIONS_FILES "Whether update the tarnslations files" OFF)
target_compile_definitions(
    ${PROJECT_NAME} PRIVATE
    $<$<BOOL:UPDATE_TRANSLATIONS_FILES>:EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES>
)

# 设置应用文件图标
set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/icons/app.ico)
if(WIN32)
    set(RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/app.rc)
    configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/template/app.rc.in
        ${RC_FILE}
        @ONLY
    )
    target_sources(${PROJECT_NAME} PRIVATE ${RC_FILE})
elseif(APPLE)
    set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_ICON_FILE "app.ico"
    )

    set_source_files_properties(
        ${APP_ICON} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )

    target_sources(${PROJECT_NAME} PRIVATE ${APP_ICON})
else()
    # todo
endif()

# 配置MacOS下Bundle属性
if(APPLE)
    set(IS_MACOSX_BUNDLE TRUE)
    set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE ${IS_MACOSX_BUNDLE}
        MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_ORGANIZATION_DOMAIN}.${APP_TITLE}
        MACOSX_BUNDLE_BUNDLE_NAME ${APP_TITLE}
        MACOSX_BUNDLE_EXECUTABLE_NAME ${APP_TITLE}
        MACOSX_BUNDLE_BUNDLE_VERSION ${APP_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${APP_VERSION}
        MACOSX_BUNDLE_COPYRIGHT ${APP_COPYRIGHT_TEXT}
    )
endif()

# 配置MacOS下语言文件
if(APPLE AND IS_MACOSX_BUNDLE)
    file(GLOB LANG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/languages/*)
    set_source_files_properties(
        ${LANG_FILES} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources/languages"
    )
    target_sources(${PROJECT_NAME} PRIVATE ${LANG_FILES})

    target_compile_definitions(${PROJECT_NAME} PRIVATE IS_MACOSX_BUNDLE)
endif()

#################
# 生成后额外命令 #
#################

# 复制语言文件至输出目录
if(NOT APPLE OR NOT IS_MACOSX_BUNDLE)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/languages ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/languages
    )
endif()
