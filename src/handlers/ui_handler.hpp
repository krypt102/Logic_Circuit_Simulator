#ifndef LOGIC_CIRCUIT_SIMULATOR_UI_HANDLER_HPP
#define LOGIC_CIRCUIT_SIMULATOR_UI_HANDLER_HPP
#include "settings_handler.hpp"

class UiHandler {
public:
    void init_window(SettingsHandler &game_settings) {
        int window_width = game_settings.get_setting<int>("screenWidth");
        int window_height = game_settings.get_setting<int>("screenHeight");

        open_window("Logic Circuit Simulator", window_width, window_height);
    }
};

#endif //LOGIC_CIRCUIT_SIMULATOR_UI_HANDLER_HPP
