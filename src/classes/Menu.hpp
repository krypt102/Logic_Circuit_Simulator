#ifndef LOGIC_CIRCUIT_SIMULATOR_MENU_HPP
#define LOGIC_CIRCUIT_SIMULATOR_MENU_HPP

class WindowHandler;
class MenuHandler;

class Menu {
public:
    virtual ~Menu() = default;

    virtual void on_enter(WindowHandler &window_handler, MenuHandler& menu_handler) = 0;
    virtual void handle_input() = 0;
    virtual void draw() const = 0;

protected:
    MenuHandler* menu_handler = nullptr;
};

#endif //LOGIC_CIRCUIT_SIMULATOR_MENU_HPP