#ifndef LOGIC_CIRCUIT_SIMULATOR_SETTINGS_MENU_HPP
#define LOGIC_CIRCUIT_SIMULATOR_SETTINGS_MENU_HPP

#include "../../classes/Menu.hpp"
#include "../../handlers/settings_handler.hpp"
#include "../../handlers/sound_handler.hpp"

class SettingsMenu : public Menu {
public:
    SettingsMenu(int window_width, int window_height, SettingsHandler& settings_handler, SoundHandler& sound_handler);

    void on_enter(WindowHandler& window_handler, MenuHandler& menu_handler) override;
    void handle_input() override;
    void draw() const override;

private:
    int window_width;
    int window_height;

    SettingsHandler& settings_handler;
    SoundHandler& sound_handler;

    WindowHandler* window_handler = nullptr;
    MenuHandler* menu_handler = nullptr;

    mutable float bg_volume;
    mutable bool show_grid;
    mutable bool snap_to_grid;

    void apply_settings() const;
};


#endif //LOGIC_CIRCUIT_SIMULATOR_SETTINGS_MENU_HPP