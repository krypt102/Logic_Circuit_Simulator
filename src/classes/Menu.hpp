#ifndef LOGIC_CIRCUIT_SIMULATOR_MENU_HPP
#define LOGIC_CIRCUIT_SIMULATOR_MENU_HPP

#include "../handlers/window_handler.hpp"
#include "../handlers/sound_handler.hpp"

class MenuHandler;

class Menu {
public:
    virtual ~Menu() = default;

    virtual void on_enter(WindowHandler& window_handler, MenuHandler& menu_handler, SoundHandler& sound_handler);
    virtual void handle_input() = 0;
    virtual void draw() const = 0;

protected:
    WindowHandler* window_handler = nullptr;
    MenuHandler* menu_handler = nullptr;
    SoundHandler* sound_handler = nullptr;
};

#endif //LOGIC_CIRCUIT_SIMULATOR_MENU_HPP