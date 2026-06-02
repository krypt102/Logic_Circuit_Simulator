#ifndef LOGIC_CIRCUIT_SIMULATOR_LOAD_FILE_MENU_HPP
#define LOGIC_CIRCUIT_SIMULATOR_LOAD_FILE_MENU_HPP

#include <string>
#include <vector>
#include "../../classes/Menu.hpp"
#include "../../handlers/settings_handler.hpp"

class LoadFileMenu : public Menu {
public:
    LoadFileMenu(int window_width, int window_height, SettingsHandler& settings_handler);

    void on_enter(WindowHandler& window_handler, MenuHandler& menu_handler) override;
    void handle_input() override;
    void draw() const override;

private:
    int window_width;
    int window_height;

    SettingsHandler& settings_handler;
    std::vector<std::string> save_file_names;

    mutable std::string error_message;

    WindowHandler* window_handler = nullptr;
    MenuHandler* menu_handler = nullptr;

    void load_save_file_names();
    void open_circuit(const std::string& filename) const;
};

#endif //LOGIC_CIRCUIT_SIMULATOR_LOAD_FILE_MENU_HPP