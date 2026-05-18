#ifndef LOGIC_CIRCUIT_SIMULATOR_NEW_FILE_MENU_HPP
#define LOGIC_CIRCUIT_SIMULATOR_NEW_FILE_MENU_HPP

#include <string>
#include "../../classes/Menu.hpp"

class NewFileMenu : public Menu {
public:
    NewFileMenu(int window_width, int window_height);

    void on_enter(WindowHandler& window_handler, MenuHandler& menu_handler) override;
    void handle_input() override;
    void draw() const override;

private:
    int window_width;
    int window_height;

    mutable std::string error_message;
    mutable std::string new_file_name;
    mutable std::string new_file_desc;

    float last_text_box_x_position = 0.0f;
    float last_text_box_y_position = 0.0f;

    WindowHandler* window_handler = nullptr;
    MenuHandler* menu_handler = nullptr;

    void try_create_project() const;
};


#endif //LOGIC_CIRCUIT_SIMULATOR_NEW_FILE_MENU_HPP
