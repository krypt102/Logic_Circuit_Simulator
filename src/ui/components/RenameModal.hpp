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

    void handle_input() override;
    void draw() const override;

private:
    std::string title;
    mutable std::string current_value;
    std::function<void(const std::string&)> on_confirm;
};

#endif // LOGIC_CIRCUIT_SIMULATOR_RENAME_MODAL_HPP