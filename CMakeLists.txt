cmake_minimum_required(VERSION 3.30)

set(FS_CLICKER_TARGET_NAME FSClicker)
set(FS_CLICKER_DISPLAY_NAME "FSClicker")
set(FS_CLICKER_VERSION 1.3.0)

project(${FS_CLICKER_TARGET_NAME} VERSION ${FS_CLICKER_VERSION} LANGUAGES CXX)

option(FS_CLICKER_STATIC_RUNTIME "Link MinGW runtime libraries statically." ON)
option(FS_CLICKER_STATIC_QT "Require a static Qt build and link Qt libraries statically." OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.11 REQUIRED COMPONENTS Core Quick Qml Multimedia)

if(FS_CLICKER_STATIC_QT)
    get_target_property(QT_CORE_TYPE Qt6::Core TYPE)
    if(QT_CORE_TYPE STREQUAL "SHARED_LIBRARY")
        message(FATAL_ERROR "FS_CLICKER_STATIC_QT requires a static Qt build. The current Qt installation is shared.")
    endif()
endif()

qt_standard_project_setup(REQUIRES 6.11)

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
        qml/Main.qml
        qml/theme/AtomTheme.qml
)

qt_add_resources(${FS_CLICKER_TARGET_NAME} app_assets
    PREFIX "/"
    FILES
        resources/application.png
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
)

target_link_libraries(${FS_CLICKER_TARGET_NAME} PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Qml
    Qt6::Multimedia
)

if(FS_CLICKER_STATIC_RUNTIME AND MINGW)
    target_link_options(${FS_CLICKER_TARGET_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
endif()

set_target_properties(${FS_CLICKER_TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${FS_CLICKER_TARGET_NAME}
)

include(GNUInstallDirs)
install(TARGETS ${FS_CLICKER_TARGET_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
