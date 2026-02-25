cmake_minimum_required(VERSION 3.17)

#############
# 收集源文件 #
#############

file(GLOB HEADER ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
if(WIN32)
    file(GLOB WIN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/platform/*_win.cpp)
    list(APPEND SRC ${WIN_SRC})
elseif(APPLE)
    file(GLOB MAC_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/*_mac.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/*_mac.mm
    )
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
)

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    global_hotkey::global_hotkey
    easy_translate::easy_translate
    Qt${QT_VERSION_MAJOR}::Widgets
)

if(APPLE)
    find_library(APPLICATION_SERVICES_LIBRARY ApplicationServices)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(SERVICE_MANAGEMENT_LIBRARY ServiceManagement)
    target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        ${APPLICATION_SERVICES_LIBRARY}
        ${COCOA_LIBRARY}
        ${SERVICE_MANAGEMENT_LIBRARY}
    )
endif()

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

# 配置应用资源/属性
set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/icons/app.ico)
if(WIN32)
    # Windows

    set(RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/app.rc)
    configure_file(
        ${CMAKE_TEMPLATE_DIR}/app.rc.in
        ${RC_FILE}
        @ONLY
    )
    target_sources(${PROJECT_NAME} PRIVATE ${RC_FILE})
elseif(APPLE)
    # Apple

    # Info Plist Strings
    set(
        INFO_PLIST_STRINGS_NAMES
        en.lproj
        zh_CN.lproj
    )
    set(CMAKE_TEMPLATE_DIR ${CMAKE_SOURCE_DIR}/cmake/template)
    foreach(INFO_PLIST_STRINGS_NAME IN LISTS INFO_PLIST_STRINGS_NAMES)
        configure_file(
            ${CMAKE_TEMPLATE_DIR}/InfoPlistStrings/${INFO_PLIST_STRINGS_NAME}/InfoPlist.strings.in
            ${CMAKE_CURRENT_BINARY_DIR}/${INFO_PLIST_STRINGS_NAME}/InfoPlist.strings
            @ONLY
        )
    endforeach()
    foreach(INFO_PLIST_STRINGS_NAME IN LISTS INFO_PLIST_STRINGS_NAMES)
        set(INFO_PLIST_STRINGS_FILE ${CMAKE_CURRENT_BINARY_DIR}/${INFO_PLIST_STRINGS_NAME}/InfoPlist.strings)
        set_source_files_properties(
            ${INFO_PLIST_STRINGS_FILE} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources/${INFO_PLIST_STRINGS_NAME}"
        )
        target_sources(${PROJECT_NAME} PRIVATE ${INFO_PLIST_STRINGS_FILE})
    endforeach()

    # Info Plist
    set(MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
    configure_file(
        ${CMAKE_TEMPLATE_DIR}/Info.plist.in
        ${MACOSX_BUNDLE_INFO_PLIST}
        @ONLY
    )

    # 应用图标
    set_source_files_properties(
        ${APP_ICON} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    target_sources(${PROJECT_NAME} PRIVATE ${APP_ICON})

    set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE ON
        MACOSX_BUNDLE_INFO_PLIST ${MACOSX_BUNDLE_INFO_PLIST}
    )

    # 语言文件
    file(GLOB LANG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/languages/*)
    set_source_files_properties(
        ${LANG_FILES} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources/languages"
    )
    target_sources(${PROJECT_NAME} PRIVATE ${LANG_FILES})
else()
    # TODO
endif()

#################
# 生成后额外命令 #
#################

# 复制语言文件至输出目录
if(NOT APPLE)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/languages ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/languages
        VERBATIM
    )
endif()

# Qt部署（需要保证*deployqt可见）
if(WIN32)
    set(DEPLOYQT windeployqt)
    set(OUTPUT_SUFFIX "exe")
elseif(APPLE)
    set(DEPLOYQT macdeployqt)
    set(OUTPUT_SUFFIX "app")
else()
    # TODO
endif()
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${DEPLOYQT}
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.${OUTPUT_SUFFIX}
    $<$<BOOL:APPLE>:-codesign=->
    VERBATIM
)

# MacOS DMG打包
option(CREATE_DMG "Create DMG for MacOS, create-dmg is required." OFF)
if(APPLE AND CREATE_DMG)
    # Qt部署与DMG打包
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/create_dmg.sh
        ${PROJECT_NAME}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        VERBATIM
    )
endif()
