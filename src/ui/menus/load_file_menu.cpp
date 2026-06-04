#include "load_file_menu.hpp"
#include "../../handlers/menu_handler.hpp"
#include "../../handlers/circuit_file_handler.hpp"
#include "../../utils/terminal_utils.h"
#include "../components/BackButton.hpp"
#include "../components/Modal.hpp"
#include "../components/UniqueButton.hpp"
#include "splashkit.h"

#include <filesystem>

#include "circuit_editor_menu.hpp"
#include "edit_circuit_details_menu.hpp"

const float FILE_BUTTON_WIDTH  = 300.0f;
const float FILE_BUTTON_HEIGHT = 48.0f;
const float FILE_BUTTON_GAP = 12.0f;
const float FILE_LIST_START_Y = 140.0f;
const float DELETE_BUTTON_WIDTH = 48.0f;
const float DELETE_BUTTON_GAP = 8.0f;
const float EDIT_BTN_WIDTH = 60.0f;
const std::string LOAD_FONT_STR = "JetBrainsMono-Regular";

LoadFileMenu::LoadFileMenu(int window_width, int window_height, SettingsHandler& settings_handler)
    : window_width(window_width),
    window_height(window_height),
    settings_handler(settings_handler)
{}

void LoadFileMenu::on_enter(WindowHandler& win_handler, MenuHandler& main_handler) {
    window_handler = &win_handler;
    menu_handler = &main_handler;
    error_message.clear();
    pending_delete_name.clear();
    pending_edit_name.clear();
    refresh_file_names();
}

void LoadFileMenu::refresh_file_names() {
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
    menu_handler->push(
        std::make_unique<CircuitEditorMenu>(std::move(*loaded_circuit), window_handler->window_width, window_handler->window_height, settings_handler)
    );
}

void LoadFileMenu::confirm_delete(const std::string& filename) {
    std::vector<ModalButton> modal_buttons;

    modal_buttons.push_back({"Delete", [this, filename]() {
        CircuitFileHandler file_handler;
        if (file_handler.delete_circuit(filename)) {
            refresh_file_names();
        } else {
            error_message = "Failed to delete '" + filename + "'.";
        }
        menu_handler->pop();
    }});

    modal_buttons.push_back({"Cancel", [this]() {
        menu_handler->pop();
    }});

    menu_handler->push(std::make_unique<Modal>(
        "Delete Circuit",
        "Delete '" + filename + "'? \nThis cannot be undone.",
        std::move(modal_buttons)
    ));
}

void LoadFileMenu::open_edit(const std::string& filename) {
    CircuitFileHandler file_handler;
    std::optional<Circuit> loaded = file_handler.load_circuit(filename);

    if (!loaded.has_value()) {
        error_message = "Failed to load '" + filename + "'.";
        return;
    }

    menu_handler->push(std::make_unique<EditCircuitDetailsMenu>(
        loaded->circuit_name,
        loaded->circuit_description,
        filename,
        [this, filename](const std::string& new_name, const std::string& new_desc) {
            CircuitFileHandler circuit_file_handler;
            std::optional<Circuit> loaded_circuit = circuit_file_handler.load_circuit(filename);
            if (!loaded_circuit.has_value()) {
                error_message = "Failed to update '" + filename + "'.";
                return;
            }
            if (new_name != filename) {
                circuit_file_handler.delete_circuit(filename);
            }
            loaded_circuit->circuit_name = new_name;
            loaded_circuit->circuit_description = new_desc;
            circuit_file_handler.save_circuit(*loaded_circuit);
            refresh_file_names();
        }
    ));
}

void LoadFileMenu::handle_input() {
    if (!pending_delete_name.empty()) {
        std::string name = pending_delete_name;
        pending_delete_name.clear();
        confirm_delete(name);
        return;
    }

    if (!pending_edit_name.empty()) {
        std::string name = pending_edit_name;
        pending_edit_name.clear();
        open_edit(name);
    }
}

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

    float row_total_width = FILE_BUTTON_WIDTH + DELETE_BUTTON_GAP + EDIT_BTN_WIDTH + DELETE_BUTTON_GAP + DELETE_BUTTON_WIDTH;
    float row_x = (window_width / 2.0f) - (row_total_width / 2.0f);

    for (int i = 0; i < (int)(save_file_names.size()); i++) {
        float button_y = FILE_LIST_START_Y + i * (FILE_BUTTON_HEIGHT + FILE_BUTTON_GAP);

        bool clicked = unique_button(save_file_names[i], rectangle_from(
            row_x,
            button_y,
            FILE_BUTTON_WIDTH,
            FILE_BUTTON_HEIGHT
        ));

        if (clicked) {
            play_sound_effect("ui_click");
            open_circuit(save_file_names[i]);
            return;
        }

        float edit_x = row_x + FILE_BUTTON_WIDTH + DELETE_BUTTON_GAP;
        bool edit_clicked = unique_button("Edit", rectangle_from(
            edit_x,
            button_y,
            EDIT_BTN_WIDTH,
            FILE_BUTTON_HEIGHT
        ));

        if (edit_clicked) {
            play_sound_effect("ui_click");
            pending_edit_name = save_file_names[i];
            return;
        }

        float delete_x = edit_x + EDIT_BTN_WIDTH + DELETE_BUTTON_GAP;
        bool delete_clicked = unique_button("X", rectangle_from(
            delete_x,
            button_y,
            DELETE_BUTTON_WIDTH,
            FILE_BUTTON_HEIGHT
        ));

        if (delete_clicked) {
            play_sound_effect("ui_click");
            pending_delete_name = save_file_names[i];
            return;
        }
    }

    if (!error_message.empty()) {
        float error_x = (window_width / 2.0f) - (text_width(error_message, LOAD_FONT_STR, 16) / 2.0f);
        draw_text(error_message, COLOR_RED, LOAD_FONT_STR, 16, error_x, window_height - 60.0f);
    }
}