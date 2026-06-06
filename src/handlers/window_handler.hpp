#ifndef LOGIC_CIRCUIT_SIMULATOR_UI_HANDLER_HPP
#define LOGIC_CIRCUIT_SIMULATOR_UI_HANDLER_HPP
#include "settings_handler.hpp"

class WindowHandler {
public:
    int window_width;
    int window_height;
    string window_name = "Logic Circuit Simulator";

    void init_window(SettingsHandler &game_settings) {
        window_width = game_settings.get_setting<int>("screenWidth");
        window_height = game_settings.get_setting<int>("screenHeight");

        open_window(window_name, window_width, window_height);
    }
};

#endif //LOGIC_CIRCUIT_SIMULATOR_UI_HANDLER_HPP
