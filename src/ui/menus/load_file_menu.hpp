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

    void refresh_file_names();

private:
    int window_width;
    int window_height;

    SettingsHandler& settings_handler;
    std::vector<std::string> save_file_names;

    mutable std::string error_message;
    mutable std::string pending_delete_name;

    WindowHandler* window_handler = nullptr;

    void open_circuit(const std::string& filename) const;
    void confirm_delete(const std::string& filename);
};

#endif //LOGIC_CIRCUIT_SIMULATOR_LOAD_FILE_MENU_HPP