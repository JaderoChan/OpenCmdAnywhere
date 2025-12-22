cmake_minimum_required(VERSION 3.17)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

file(GLOB HEADER *.h *.hpp)
file(GLOB SRC *.c *.cpp)
file(GLOB UI *.ui)
file(GLOB QRC *.qrc)
set(PROJECT_SOURCE ${HEADER} ${SRC} ${UI} ${QRC})

qt_standard_project_setup()
qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCE})

set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icon/icon.ico")
if(WIN32)
    set(RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/app_icon.rc")
    file(WRITE ${RC_FILE} "IDI_ICON1 ICON \"${APP_ICON}\"")
    target_sources(${PROJECT_NAME} PRIVATE ${RC_FILE})
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE app_icon.icns)
    set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${json_SOURCE_DIR}/include
    ${easy_translate_SOURCE_DIR}/include
    ${minilog_SOURCE_DIR}/include
)
target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    global_hotkey::global_hotkey
    Qt${QT_VERSION_MAJOR}::Widgets
)

set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-${ARCH_SUFFIX}"
)

target_compile_definitions(
    ${PROJECT_NAME} PRIVATE
    $<$<BOOL:UPDATE_TRANSLATIONS_FILES>:EASY_TRANSLATE_UPDATE_TRANSLATIONS_FILES>
    $<$<BOOL:OCAW_OUTLOG>:OCAW_OUTLOG>
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY language DESTINATION ${CMAKE_INSTALL_BINDIR})
