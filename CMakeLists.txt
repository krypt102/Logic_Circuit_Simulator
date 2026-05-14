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
        src/handlers/window_handler.hpp
        src/classes/Menu.hpp
        src/handlers/menu_handler.cpp
        src/handlers/menu_handler.hpp
        src/handlers/font_handler.hpp
        src/handlers/sound_handler.hpp
        src/ui/menus/main_menu.hpp
        src/ui/menus/main_menu.cpp
        src/ui/components/BackButton.cpp
        src/ui/components/BackButton.hpp
        src/ui/menus/new_file_menu.cpp
        src/ui/menus/new_file_menu.hpp
        src/classes/Pin.hpp
        src/classes/Pin.hpp
        src/classes/Pin.hpp)
target_link_libraries(Logic_Circuit_Simulator PRIVATE SplashKit)