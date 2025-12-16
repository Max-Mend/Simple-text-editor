cmake_minimum_required(VERSION 3.16)
project(chora-spatium VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

include_directories(${Python3_INCLUDE_DIRS})

add_executable(chora-spatium
        main.cpp
        core/linenum.h
        core/codeeditor.cpp
        core/codeeditor.h
        ui/MainWindow.cpp
        ui/MainWindow.h
        ui/MenuBar.cpp
        ui/MenuBar.h
        ui/SettingsDialog.cpp
        ui/SettingsDialog.h
        ui/AboutDialog.cpp
        ui/AboutDialog.h
        ui/StatusBar.cpp
        ui/StatusBar.h
)

target_link_libraries(chora-spatium
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        ${Python3_LIBRARIES}
)

target_compile_definitions(chora-spatium PRIVATE
        PYTHON_PLUGINS_PATH="${CMAKE_SOURCE_DIR}/plugins"
)