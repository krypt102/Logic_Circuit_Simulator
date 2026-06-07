#ifndef LOGIC_CIRCUIT_SIMULATOR_WINDOW_HANDLER_HPP
#define LOGIC_CIRCUIT_SIMULATOR_WINDOW_HANDLER_HPP

#include <string>
#include "splashkit.h"
#include "settings_handler.hpp"

class WindowHandler {
public:
    int window_width = 0;
    int window_height = 0;
    bool window_closed = false;

    const std::string WINDOW_NAME = "Logic Circuit Simulator";

    void init_window(SettingsHandler& game_settings) {
        window_width  = game_settings.get_setting<int>("screenWidth");
        window_height = game_settings.get_setting<int>("screenHeight");
        open_window(WINDOW_NAME, window_width, window_height);
    }

    void close_game_window() {
        window_closed = true;
        close_window(WINDOW_NAME);
    }
};

#endif // LOGIC_CIRCUIT_SIMULATOR_WINDOW_HANDLER_HPP
