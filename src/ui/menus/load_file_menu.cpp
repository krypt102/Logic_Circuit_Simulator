#include "load_file_menu.hpp"
#include "../../handlers/menu_handler.hpp"
#include "../../handlers/circuit_file_handler.hpp"
#include "../../utils/terminal_utils.h"
#include "../components/BackButton.hpp"
#include "splashkit.h"

#include <filesystem>

const float FILE_BUTTON_WIDTH  = 440.0f;
const float FILE_BUTTON_HEIGHT = 48.0f;
const float FILE_BUTTON_GAP = 12.0f;
const float FILE_LIST_START_Y = 140.0f;
const std::string LOAD_FONT_STR = "JetBrainsMono-Regular";

LoadFileMenu::LoadFileMenu(int window_width, int window_height)
    : window_width(window_width), window_height(window_height) {}

void LoadFileMenu::on_enter(WindowHandler& win_handler, MenuHandler& main_handler) {
    window_handler = &win_handler;
    menu_handler = &main_handler;
    error_message.clear();
    load_save_file_names();
}

void LoadFileMenu::load_save_file_names() {
    save_file_names.clear();

    if (!std::filesystem::exists(SAVES_FOLDER)) {
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(SAVES_FOLDER)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        std::string filename = entry.path().filename().string();
        if (
            filename.size() > CIRCUIT_FILE_EXTENSION.size() &&
            filename.substr(filename.size() - CIRCUIT_FILE_EXTENSION.size()) == CIRCUIT_FILE_EXTENSION
    ) {
            save_file_names.push_back(filename.substr(0, filename.size() - CIRCUIT_FILE_EXTENSION.size()));
        }
    }
}

void LoadFileMenu::open_circuit(const std::string& filename) const {
    CircuitFileHandler file_handler;
    std::optional<Circuit> loaded_circuit = file_handler.load_circuit(filename);

    if (!loaded_circuit.has_value()) {
        error_message = "Failed to load '" + filename + "'. The file may be corrupted.";
        print_error("LoadFileMenu: failed to load circuit: " + filename);
        return;
    }

    print_info("LoadFileMenu: circuit loaded: " + loaded_circuit->circuit_name);

    print_warning("This is where the editor will load");
    error_message = "Editor not yet done.";
}

void LoadFileMenu::handle_input() {}

void LoadFileMenu::draw() const {
    bool has_clicked_back = draw_back_button();
    if (has_clicked_back) {
        play_sound_effect("ui_click");
        menu_handler->pop();
        return;
    }

    std::string title_text = "Load Circuit";
    int title_font_size = 28;
    float title_x = (window_width / 2.0f) - (text_width(title_text, LOAD_FONT_STR, title_font_size) / 2.0f);
    draw_text(title_text, COLOR_BLACK, LOAD_FONT_STR, title_font_size, title_x, 60.0f);

    if (save_file_names.empty()) {
        std::string no_files_msg = "No saved circuits found.";
        float msg_x = (window_width / 2.0f) - (text_width(no_files_msg, LOAD_FONT_STR, 22) / 2.0f);
        draw_text(no_files_msg, COLOR_GRAY, LOAD_FONT_STR, 22, msg_x, window_height / 2.0f);
        return;
    }

    float button_x = (window_width / 2.0f) - (FILE_BUTTON_WIDTH / 2.0f);

    for (int i = 0; i < static_cast<int>(save_file_names.size()); i++) {
        float button_y = FILE_LIST_START_Y + i * (FILE_BUTTON_HEIGHT + FILE_BUTTON_GAP);

        bool clicked = button(save_file_names[i], rectangle_from(
            button_x,
            button_y,
            FILE_BUTTON_WIDTH,
            FILE_BUTTON_HEIGHT
        ));

        if (clicked) {
            play_sound_effect("ui_click");
            open_circuit(save_file_names[i]);
            return;
        }
    }

    if (!error_message.empty()) {
        float error_x = (window_width / 2.0f) - (text_width(error_message, LOAD_FONT_STR, 16) / 2.0f);
        draw_text(error_message, COLOR_RED, LOAD_FONT_STR, 16, error_x, window_height - 60.0f);
    }
}