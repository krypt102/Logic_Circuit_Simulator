#ifndef LOGIC_CIRCUIT_SIMULATOR_MODAL_HPP
#define LOGIC_CIRCUIT_SIMULATOR_MODAL_HPP

#include <functional>
#include <string>
#include <vector>
#include "../../classes/Menu.hpp"

struct ModalButton {
    std::string label;
    std::function<void()> on_click;
};

class Modal : public Menu {
public:
    Modal(std::string title, std::string message, std::vector<ModalButton> buttons);

    void on_enter(WindowHandler& window_handler, MenuHandler& menu_handler) override;
    void handle_input() override;
    void draw() const override;

private:
    std::string title;
    std::string message;
    std::vector<ModalButton> buttons;

    int window_width = 0;
    int window_height = 0;

    WindowHandler* window_handler = nullptr;
};

#endif //LOGIC_CIRCUIT_SIMULATOR_MODAL_HPP