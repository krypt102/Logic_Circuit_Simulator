#ifndef LOGIC_CIRCUIT_SIMULATOR_SETTINGS_MENU_HPP
#define LOGIC_CIRCUIT_SIMULATOR_SETTINGS_MENU_HPP

#include "../../classes/Menu.hpp"
#include "../../handlers/settings_handler.hpp"

class SettingsMenu : public Menu {
public:
    SettingsMenu(SettingsHandler& settings_handler);

    void on_enter(WindowHandler& window_handler, MenuHandler& menu_handler, SoundHandler& sound_handler) override;
    void handle_input() override;
    void draw() const override;

private:
    SettingsHandler& settings_handler;

    mutable float bg_volume;
    mutable float sfx_volume;
    mutable bool show_grid;
    mutable bool snap_to_grid;
    mutable int selected_res_index;
    mutable int monitor_width;
    mutable int monitor_height;

    void apply_settings() const;
};

#endif //LOGIC_CIRCUIT_SIMULATOR_SETTINGS_MENU_HPP