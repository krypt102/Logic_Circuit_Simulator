#ifndef LOGIC_CIRCUIT_SIMULATOR_MENU_HANDLER
#define LOGIC_CIRCUIT_SIMULATOR_MENU_HANDLER

#include <memory>
#include <vector>

#include "../utils/terminal_utils.h"
#include "../classes/Menu.hpp"
#include "../handlers/window_handler.hpp"

// Manages a list of menus treated as a "stack" (a dynamic_array).
// push() - go to a new menu, and place it at the top of the stack
// pop() - pop the top of the stack off and revert to the previous menu
// update() - run handle_input() and draw() for the menu each frame.

class MenuHandler {
    public:
        MenuHandler(WindowHandler &win_handler) : window_handler(win_handler) {
            print_info("Initialised menu handler");
        } ;

        void push(std::unique_ptr<Menu> menu);
        void pop();
        void update() const;

    private:
        std::vector<std::unique_ptr<Menu>> menus;
        WindowHandler &window_handler;
        Menu* current_menu() const;
};

#endif