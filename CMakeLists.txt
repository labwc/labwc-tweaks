cmake_minimum_required(VERSION 3.5)

project(labwc-tweaks VERSION 0.0 LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=undefined")

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
find_package(LibXml2 REQUIRED)

qt_add_executable(${CMAKE_PROJECT_NAME}
    MANUAL_FINALIZATION
    tweaks-qt/main.cpp
    tweaks-qt/mainwindow.cpp
    tweaks-qt/mainwindow.h
    tweaks-qt/mainwindow.ui
    environment.c
    environment.h
    theme.c
    theme.h
    xml.c
    xml.h
    keyboard-layouts.c
    keyboard-layouts.h
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE .)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${GLIB_INCLUDE_DIRS})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${LIBXML2_INCLUDE_DIR})

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Widgets ${GLIB_LDFLAGS} ${LIBXML2_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} INTERFACE ${GLIB_LDFLAGS})

#target_link_options(${CMAKE_PROJECT_NAME} BEFORE PUBLIC -fsanitize=undefined PUBLIC -fsanitize=address)

include(GNUInstallDirs)
install(TARGETS ${CMAKE_PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(${CMAKE_PROJECT_NAME})
