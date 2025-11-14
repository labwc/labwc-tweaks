cmake_minimum_required(VERSION 3.5)
project(labwc-tweaks
    VERSION 0.1.0
    LANGUAGES CXX
)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(Config.cmake)

# QtCreator doesn't use the system locale and I see no way to prefix with LANG=XYZ.UTF-8 the command,
# so enabling these 2 settings we can test any language, see initLocale() in main.cpp.
set(PROJECT_TRANSLATION_TEST_ENABLED 0 CACHE STRING "Whether to enable translation testing [default: 0]")
set(PROJECT_TRANSLATION_TEST_LANGUAGE "en" CACHE STRING "Country code of language to test in IDE [default: en]")
set(PROJECT_QT_VERSION 6 CACHE STRING "Qt version to use [Default: 6]")
option(PROJECT_TRANSLATIONS_UPDATE "Update source translations [default: OFF]" OFF)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=undefined")

find_package(QT NAMES Qt${PROJECT_QT_VERSION})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
find_package(LibXml2 REQUIRED)

set(PROJECT_SOURCES
    src/main.cpp
    src/maindialog.ui
    src/maindialog.cpp
    src/maindialog.h
    src/layoutmodel.cpp
    src/layoutmodel.h
    src/environment.cpp
    src/environment.h
    src/settings.cpp
    src/settings.h
    src/xml.cpp
    src/xml.h
    src/appearance.cpp
    src/appearance.h
    src/appearance.ui
    src/find-themes.cpp
    src/find-themes.h
)
set(PROJECT_OTHER_FILES
    .github/workflows/build.yml
    README.md
)
file(GLOB PROJECT_TRANSLATION_SOURCES "${PROJECT_TRANSLATIONS_DIR}/*")
source_group("" FILES
    ${PROJECT_SOURCES}
    ${PROJECT_TRANSLATION_SOURCES}
)
#===================================================================================================
# Translations
#===================================================================================================
include(GNUInstallDirs)
include(LXQtTranslate)

lxqt_translate_ts(PROJECT_QM_FILES
    SOURCES             ${PROJECT_SOURCES}
    TEMPLATE            ${PROJECT_ID}
    TRANSLATION_DIR     "${PROJECT_TRANSLATIONS_DIR}"
    UPDATE_TRANSLATIONS ${PROJECT_TRANSLATIONS_UPDATE}
    INSTALL_DIR         "${CMAKE_INSTALL_DATADIR}/${PROJECT_ID}/translations"
)
lxqt_translate_desktop(PROJECT_DESKTOP_FILES
    SOURCES         "${CMAKE_BINARY_DIR}/${PROJECT_APPSTREAM_ID}.desktop.in"
    TRANSLATION_DIR "${PROJECT_TRANSLATIONS_DIR}"
    USE_YAML
)

#===================================================================================================
# Tests
#===================================================================================================
include(CTest)

add_executable(t1000 tests/t1000-add-xpath-node.cpp tests/tap.cpp src/xml.cpp)
target_link_libraries(t1000 PRIVATE ${GLIB_LDFLAGS} ${LIBXML2_LIBRARIES})
target_include_directories(t1000 PRIVATE ${GLIB_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIR})
add_test(t1000 t1000)

add_executable(t1001 tests/t1001-nodenames.cpp tests/tap.cpp)
target_link_libraries(t1001 PRIVATE ${GLIB_LDFLAGS} ${LIBXML2_LIBRARIES})
target_include_directories(t1001 PRIVATE ${GLIB_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIR})
add_test(t1001 t1001)

#===================================================================================================
# Application
#===================================================================================================
qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${PROJECT_DESKTOP_FILES}
    ${PROJECT_OTHER_FILES}
    ${PROJECT_QM_FILES}
    ${PROJECT_TRANSLATION_SOURCES}
)
set(PROJECT_ICON_SYSTEM_PATH "${CMAKE_INSTALL_FULL_DATADIR}/icons/hicolor/scalable/apps")
file(COPY_FILE "${CMAKE_SOURCE_DIR}/data/${PROJECT_APPSTREAM_ID}.svg"
    "${CMAKE_BINARY_DIR}/${PROJECT_APPSTREAM_ID}.svg"
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    APPLICATION_NAME="${PROJECT_NAME}"
    APPLICATION_VERSION="${PROJECT_VERSION}"
    PROJECT_ID="${PROJECT_ID}"
    PROJECT_APPSTREAM_ID="${PROJECT_APPSTREAM_ID}"
    PROJECT_DATA_DIR="${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}"
    PROJECT_ICON_SYSTEM_PATH="${PROJECT_ICON_SYSTEM_PATH}"
    PROJECT_TRANSLATION_TEST_ENABLED=${PROJECT_TRANSLATION_TEST_ENABLED}
    PROJECT_TRANSLATION_TEST_LANGUAGE="${PROJECT_TRANSLATION_TEST_LANGUAGE}"
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${GLIB_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
    src
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${GLIB_LDFLAGS}
    ${LIBXML2_LIBRARIES}
)
#target_link_options(${PROJECT_NAME} BEFORE PUBLIC -fsanitize=undefined PUBLIC -fsanitize=address)
#===================================================================================================
# Installation
#===================================================================================================
configure_file("${CMAKE_SOURCE_DIR}/data/${PROJECT_APPSTREAM_ID}.desktop.in"
    "${CMAKE_BINARY_DIR}/${PROJECT_APPSTREAM_ID}.desktop.in" @ONLY
)
configure_file("${CMAKE_SOURCE_DIR}/data/${PROJECT_APPSTREAM_ID}.appdata.xml.in"
    "${CMAKE_BINARY_DIR}/${PROJECT_APPSTREAM_ID}.appdata.xml" @ONLY
)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_APPSTREAM_ID}.appdata.xml"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/metainfo"
)
install(FILES "${PROJECT_DESKTOP_FILES}"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
)
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_APPSTREAM_ID}.svg"
    # Don't use PROJECT_ICON_SYSTEM_PATH here which is absolute and doesn't take prefixes into account
    DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps"
)
qt_finalize_executable(${PROJECT_NAME})
#===================================================================================================
# Configuration report
#===================================================================================================
message(STATUS "
Project name:                     ${PROJECT_NAME}
Version:                          ${PROJECT_VERSION}
Qt version:                       ${QT_VERSION}
Build type:                       ${CMAKE_BUILD_TYPE}
Install prefix:                   ${CMAKE_INSTALL_PREFIX}
Update translations before build: ${PROJECT_TRANSLATIONS_UPDATE}
")
