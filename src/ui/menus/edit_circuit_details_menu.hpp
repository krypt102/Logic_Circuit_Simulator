#ifndef LOGIC_CIRCUIT_SIMULATOR_EDIT_CIRCUIT_DETAILS_MENU_HPP
#define LOGIC_CIRCUIT_SIMULATOR_EDIT_CIRCUIT_DETAILS_MENU_HPP

#include <string>
#include <functional>
#include "../../classes/Menu.hpp"

class EditCircuitDetailsMenu : public Menu {
public:
    EditCircuitDetailsMenu(
        std::string current_name,
        std::string current_description,
        std::string original_name,
        std::function<void(const std::string&, const std::string&)> on_save
    );

    void handle_input() override;
    void draw() const override;

private:
    std::string name_value;
    std::string description_value;
    std::string original_name;
    std::function<void(const std::string&, const std::string&)> on_save;

    mutable std::string error_message;
    mutable bool pending_save = false;
    mutable bool pending_cancel = false;
};

#endif //LOGIC_CIRCUIT_SIMULATOR_EDIT_CIRCUIT_DETAILS_MENU_HPP