cmake_minimum_required(VERSION 3.30)

set(FS_CLICKER_TARGET_NAME FSClicker)
set(FS_CLICKER_DISPLAY_NAME "FSClicker")
set(FS_CLICKER_VERSION 1.3.1)
set(FS_CLICKER_AUTHOR_NAME "Flowersauce")
set(FS_CLICKER_REPOSITORY_URL "https://github.com/flowersauce/FSClicker")

project(${FS_CLICKER_TARGET_NAME} VERSION ${FS_CLICKER_VERSION} LANGUAGES CXX)

option(FS_CLICKER_STATIC_RUNTIME "Link MinGW runtime libraries statically." ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.11 REQUIRED COMPONENTS Core Quick Qml Svg)

qt_standard_project_setup(REQUIRES 6.11)

set_source_files_properties(qml/main.qml PROPERTIES QT_QML_SOURCE_TYPENAME Main)
set_source_files_properties(qml/components/config_row.qml PROPERTIES QT_QML_SOURCE_TYPENAME ConfigRow)
set_source_files_properties(qml/components/nav_button.qml PROPERTIES QT_QML_SOURCE_TYPENAME NavButton)
set_source_files_properties(qml/components/segment_button.qml PROPERTIES QT_QML_SOURCE_TYPENAME SegmentButton)
set_source_files_properties(qml/components/small_action_button.qml PROPERTIES QT_QML_SOURCE_TYPENAME SmallActionButton)
set_source_files_properties(qml/components/traffic_button.qml PROPERTIES QT_QML_SOURCE_TYPENAME TrafficButton)
set_source_files_properties(qml/theme/atom_theme.qml PROPERTIES QT_QML_SOURCE_TYPENAME AtomTheme)

qt_add_executable(${FS_CLICKER_TARGET_NAME}
        WIN32
        src/main.cpp
        src/app_config.cpp
        src/clicker_controller.cpp
        src/event_injector.cpp
        src/global_definitions.cpp
        src/keyboard_hook.cpp
        include/app_config.h
        include/clicker_controller.h
        include/event_injector.h
        include/global_definitions.h
        include/keyboard_hook.h
        resources/app_icon.rc
)

qt_add_qml_module(${FS_CLICKER_TARGET_NAME}
        URI FSClicker
        VERSION 1.0
        QML_FILES
        qml/main.qml
        qml/components/config_row.qml
        qml/components/nav_button.qml
        qml/components/segment_button.qml
        qml/components/small_action_button.qml
        qml/components/traffic_button.qml
        qml/theme/atom_theme.qml
)

qt_add_resources(${FS_CLICKER_TARGET_NAME} app_assets
        PREFIX "/"
        FILES
        resources/FSClicker_transparent.svg
        resources/Jura-Medium.ttf
        resources/SarasaUiSC-Regular.ttf
        resources/open.wav
        resources/close.wav
)

target_include_directories(${FS_CLICKER_TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(${FS_CLICKER_TARGET_NAME} PRIVATE
        FS_CLICKER_APP_NAME="${FS_CLICKER_DISPLAY_NAME}"
        FS_CLICKER_APP_VERSION="${PROJECT_VERSION}"
        FS_CLICKER_AUTHOR_NAME="${FS_CLICKER_AUTHOR_NAME}"
        FS_CLICKER_REPOSITORY_URL="${FS_CLICKER_REPOSITORY_URL}"
)

target_link_libraries(${FS_CLICKER_TARGET_NAME} PRIVATE
        Qt6::Core
        Qt6::Quick
        Qt6::Qml
        Qt6::Svg
        winmm
)

if (FS_CLICKER_STATIC_RUNTIME AND MINGW)
    target_link_options(${FS_CLICKER_TARGET_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
endif ()

set_target_properties(${FS_CLICKER_TARGET_NAME} PROPERTIES
        OUTPUT_NAME ${FS_CLICKER_TARGET_NAME}
)

include(GNUInstallDirs)
install(TARGETS ${FS_CLICKER_TARGET_NAME}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
