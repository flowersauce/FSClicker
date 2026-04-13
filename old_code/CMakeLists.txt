cmake_minimum_required(VERSION 3.16)

set(FS_CLICKER_TARGET_NAME FSClicker)
set(FS_CLICKER_DISPLAY_NAME "FS Clicker")
set(FS_CLICKER_VERSION 1.2.2)

project(${FS_CLICKER_TARGET_NAME} VERSION ${FS_CLICKER_VERSION} LANGUAGES CXX)

option(FS_CLICKER_STATIC_RUNTIME "Link MinGW runtime libraries statically." ON)
option(FS_CLICKER_STATIC_QT "Require a static Qt build and link Qt libraries statically." OFF)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

if(FS_CLICKER_STATIC_QT)
    get_target_property(QT_CORE_TYPE Qt${QT_VERSION_MAJOR}::Core TYPE)
    if(QT_CORE_TYPE STREQUAL "SHARED_LIBRARY")
        message(FATAL_ERROR "FS_CLICKER_STATIC_QT requires a static Qt build. The current Qt installation is shared.")
    endif()
endif()

set(PROJECT_SOURCES
        src/main.cpp
        src/main_window.cpp
        src/custom_widget.cpp
        src/coordinate_capture_window.cpp
        src/event_injector.cpp
        src/global_definitions.cpp
        src/keyboard_hook.cpp

        include/main_window.h
        include/custom_widget.h
        include/coordinate_capture_window.h
        include/event_injector.h
        include/global_definitions.h
        include/keyboard_hook.h

        ui/main_window.ui
        ui/custom_widget.ui

        resources/app_icon.rc
        resources/app_resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${FS_CLICKER_TARGET_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET FlowersauceClicker APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${FS_CLICKER_TARGET_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${FS_CLICKER_TARGET_NAME}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(${FS_CLICKER_TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
)

target_compile_definitions(${FS_CLICKER_TARGET_NAME} PRIVATE
    FS_CLICKER_APP_NAME="${FS_CLICKER_DISPLAY_NAME}"
    FS_CLICKER_APP_VERSION="${PROJECT_VERSION}"
)

target_link_libraries(${FS_CLICKER_TARGET_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)

if(FS_CLICKER_STATIC_RUNTIME AND MINGW)
    target_link_options(${FS_CLICKER_TARGET_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.flowersauce.fsclicker)
endif()
set_target_properties(${FS_CLICKER_TARGET_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    OUTPUT_NAME ${FS_CLICKER_TARGET_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${FS_CLICKER_TARGET_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${FS_CLICKER_TARGET_NAME})
endif()
