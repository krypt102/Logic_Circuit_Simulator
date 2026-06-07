#ifndef LOGIC_CIRCUIT_SIMULATOR_MAIN_MENU_HPP
#define LOGIC_CIRCUIT_SIMULATOR_MAIN_MENU_HPP

#include "../../classes/Menu.hpp"
#include "../../handlers/settings_handler.hpp"

class MainMenu : public Menu {
public:
    MainMenu(SettingsHandler& settings_handler);

    void handle_input() override;
    void draw() const override;

private:
    SettingsHandler& settings_handler;
};

#endif // LOGIC_CIRCUIT_SIMULATOR_MAIN_MENU_HPP