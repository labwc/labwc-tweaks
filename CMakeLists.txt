cmake_minimum_required(VERSION 3.5)
project(labwc-tweaks
    VERSION 0.1.0
    LANGUAGES C CXX
)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(PROJECT_QT_VERSION 6 CACHE STRING "Qt version to use [Default: 6]")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=undefined")

find_package(QT NAMES Qt${PROJECT_QT_VERSION})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
find_package(LibXml2 REQUIRED)

set(PROJECT_SOURCES
    tweaks-qt/main.cpp
    tweaks-qt/maindialog.ui
    tweaks-qt/maindialog.cpp
    tweaks-qt/maindialog.h
    environment.c
    environment.h
    theme.c
    theme.h
    xml.c
    xml.h
    keyboard-layouts.c
    keyboard-layouts.h
)
source_group("" FILES ${PROJECT_SOURCES})

qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLIB_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBXML2_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${GLIB_LDFLAGS}
    ${LIBXML2_LIBRARIES}
)
#target_link_options(${PROJECT_NAME} BEFORE PUBLIC -fsanitize=undefined PUBLIC -fsanitize=address)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
qt_finalize_executable(${PROJECT_NAME})
