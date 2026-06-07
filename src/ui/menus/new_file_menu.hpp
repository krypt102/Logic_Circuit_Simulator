#ifndef LOGIC_CIRCUIT_SIMULATOR_NEW_FILE_MENU_HPP
#define LOGIC_CIRCUIT_SIMULATOR_NEW_FILE_MENU_HPP

#include <string>
#include "../../classes/Menu.hpp"
#include "../../handlers/settings_handler.hpp"

class NewFileMenu : public Menu {
public:
    NewFileMenu(SettingsHandler& settings_handler);

    void on_enter(WindowHandler& window_handler, MenuHandler& menu_handler, SoundHandler& sound_handler) override;
    void handle_input() override;
    void draw() const override;

private:
    SettingsHandler& settings_handler;

    mutable std::string circuit_name;
    mutable std::string circuit_description;
    mutable std::string error_message;

    mutable float last_field_x = 0.0f;
    mutable float last_field_y = 0.0f;

    void try_create_circuit() const;
};

#endif // LOGIC_CIRCUIT_SIMULATOR_NEW_FILE_MENU_HPP