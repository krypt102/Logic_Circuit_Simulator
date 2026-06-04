#ifndef LOGIC_CIRCUIT_SIMULATOR_RENAME_MODAL_HPP
#define LOGIC_CIRCUIT_SIMULATOR_RENAME_MODAL_HPP

#include <functional>
#include <string>
#include "../../classes/Menu.hpp"

class RenameModal : public Menu {
public:
    RenameModal(
        std::string title,
        std::string initial_value,
        std::function<void(const std::string&)> on_confirm
    );

    void on_enter(WindowHandler& window_handler, MenuHandler& menu_handler) override;
    void handle_input() override;
    void draw() const override;

private:
    std::string title;
    mutable std::string current_value;
    std::function<void(const std::string&)> on_confirm;

    int window_width  = 0;
    int window_height = 0;

    WindowHandler* window_handler = nullptr;
    MenuHandler*   menu_handler   = nullptr;
};

#endif // LOGIC_CIRCUIT_SIMULATOR_RENAME_MODAL_HPP