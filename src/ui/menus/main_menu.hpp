#ifndef LOGIC_CIRCUIT_SIMULATOR_MAIN_MENU_HPP
#define LOGIC_CIRCUIT_SIMULATOR_MAIN_MENU_HPP

#include "../../classes/Menu.hpp"
#include "../../handlers/window_handler.hpp"

class MainMenu : public Menu {
public:
    MainMenu(int window_width, int window_height);

    void on_enter(WindowHandler& window_handler, MenuHandler& menu_handler) override;
    void handle_input() override;
    void draw() const override;

private:
    int window_width;
    int window_height;

    WindowHandler* window_handler{};
    MenuHandler* menu_handler{};
};

#endif //LOGIC_CIRCUIT_SIMULATOR_MAIN_MENU_HPP
