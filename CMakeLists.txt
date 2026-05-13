cmake_minimum_required(VERSION 4.2)
project(Logic_Circuit_Simulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/execs)

add_executable(Logic_Circuit_Simulator
        src/main.cpp
        src/utils/utilities.cpp
        src/utils/terminal_utils.cpp
        src/utils/terminal_utils.h
        src/handlers/settings_handler.hpp
        src/handlers/ui_handler.hpp)
target_link_libraries(Logic_Circuit_Simulator PRIVATE SplashKit)