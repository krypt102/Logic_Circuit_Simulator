#include "load_file_menu.hpp"
#include "../../handlers/menu_handler.hpp"
#include "../../handlers/circuit_file_handler.hpp"
#include "../../utils/terminal_utils.h"
#include "../../utils/utilities.h"
#include "../components/BackButton.hpp"
#include "../components/Modal.hpp"
#include "../components/UniqueButton.hpp"
#include "splashkit.h"

#include <algorithm>
#include <filesystem>

#include "circuit_editor_menu.hpp"
#include "edit_circuit_details_menu.hpp"

const float FILE_BUTTON_WIDTH = 300.0f;
const float FILE_BUTTON_HEIGHT = 48.0f;
const float FILE_DESC_FONT_SIZE = 12.0f;
const float FILE_DESC_LINE_HEIGHT = 16.0f;
const float FILE_ROW_DESC_PADDING = 6.0f;
const float FILE_BUTTON_GAP = 12.0f;
const float FILE_LIST_START_Y = 140.0f;
const float DELETE_BUTTON_WIDTH = 48.0f;
const float DELETE_BUTTON_GAP = 8.0f;
const float EDIT_BTN_WIDTH = 60.0f;
const std::string LOAD_FONT_STR = "JetBrainsMono-Regular";

LoadFileMenu::LoadFileMenu(SettingsHandler& settings_handler)
    : settings_handler(settings_handler)
{
    print_info("LoadFileMenu Initialized");
}

void LoadFileMenu::on_enter(WindowHandler& win_handler, MenuHandler& main_handler, SoundHandler& snd_handler) {
    Menu::on_enter(win_handler, main_handler, snd_handler);
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

    CircuitFileHandler file_handler;
    for (const auto& entry : std::filesystem::directory_iterator(SAVES_FOLDER)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        std::string filename = entry.path().filename().string();
        if (
            filename.size() > CIRCUIT_FILE_EXTENSION.size() &&
            filename.substr(filename.size() - CIRCUIT_FILE_EXTENSION.size()) == CIRCUIT_FILE_EXTENSION
        ) {
            std::string name = filename.substr(0, filename.size() - CIRCUIT_FILE_EXTENSION.size());
            std::optional<Circuit> loaded = file_handler.load_circuit(name);
            std::string desc = loaded.has_value() ? loaded->circuit_description : "";
            save_file_names.emplace_back(name, desc);
        }
    }
    std::sort(save_file_names.begin(), save_file_names.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });
}

void LoadFileMenu::open_circuit(const std::string& filename) const {
    CircuitFileHandler file_handler;
    std::optional<Circuit> loaded_circuit = file_handler.load_circuit(filename);

    if (!loaded_circuit.has_value()) {
        error_message = std::format(R"(Failed to load "{}". The file may be corrupted.)", filename);
        print_error(std::format("LoadFileMenu: failed to load circuit {}", filename));
        return;
    }

    print_info(std::format("LoadFileMenu: Circuit loaded - {}", loaded_circuit->circuit_name));
    menu_handler->push(
        std::make_unique<CircuitEditorMenu>(std::move(*loaded_circuit), settings_handler)
    );
}

void LoadFileMenu::confirm_delete(const std::string& filename) {
    std::vector<ModalButton> modal_buttons;

    modal_buttons.push_back({"Delete", [this, filename]() {
        CircuitFileHandler file_handler;
        if (file_handler.delete_circuit(filename)) {
            refresh_file_names();
        } else {
            error_message = std::format(R"(Failed to delete "{}")", filename);
        }
        menu_handler->pop();
    }});

    modal_buttons.push_back({"Cancel", [this]() {
        menu_handler->pop();
    }});

    menu_handler->push(std::make_unique<Modal>(
        "Delete Circuit",
        std::format(R"(Delete "{}"? \nThis cannot be undone.)", filename),
        std::move(modal_buttons)
    ));
}

void LoadFileMenu::open_edit(const std::string& filename) {
    CircuitFileHandler file_handler;
    std::optional<Circuit> loaded = file_handler.load_circuit(filename);

    if (!loaded.has_value()) {
        error_message = std::format(R"(Failed to load "{}")", filename);
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
                error_message = std::format(R"(Failed to update "{}")", filename);
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
        sound_handler->play_sfx("ui_click");
        menu_handler->pop();
        return;
    }

    std::string title_text = "Load Circuit";
    int title_font_size = 28;
    float title_x = (window_handler->window_width / 2.0f) - (text_width(title_text, LOAD_FONT_STR, title_font_size) / 2.0f);
    draw_text(title_text, COLOR_BLACK, LOAD_FONT_STR, title_font_size, title_x, 60.0f);

    if (save_file_names.empty()) {
        std::string no_files_msg = "No saved circuits found.";
        float msg_x = (window_handler->window_width / 2.0f) - (text_width(no_files_msg, LOAD_FONT_STR, 22) / 2.0f);
        draw_text(no_files_msg, COLOR_GRAY, LOAD_FONT_STR, 22, msg_x, window_handler->window_height / 2.0f);
        return;
    }

    float row_total_width = FILE_BUTTON_WIDTH + DELETE_BUTTON_GAP + EDIT_BTN_WIDTH + DELETE_BUTTON_GAP + DELETE_BUTTON_WIDTH;
    float row_x = (window_handler->window_width / 2.0f) - (row_total_width / 2.0f);

    float container_top = FILE_LIST_START_Y;
    float container_bottom = window_handler->window_height - 80.0f;
    float container_height = container_bottom - container_top;
    float container_padding = 8.0f;

    rectangle container_rect = rectangle_from(
        row_x - container_padding,
        container_top,
        row_total_width + container_padding * 2,
        container_height
    );

    auto row_height_for = [&](const std::pair<std::string, std::string>& entry) -> float {
        if (entry.second.empty()) {
            return FILE_BUTTON_HEIGHT;
        }
        std::vector<std::string> desc_lines = wrap_text(entry.second, row_total_width, LOAD_FONT_STR, (int)FILE_DESC_FONT_SIZE);
        return FILE_BUTTON_HEIGHT + FILE_ROW_DESC_PADDING + (int)desc_lines.size() * FILE_DESC_LINE_HEIGHT;
    };

    float total_content_height = 0.0f;
    for (const auto& entry : save_file_names) {
        total_content_height += row_height_for(entry) + FILE_BUTTON_GAP;
    }
    total_content_height -= FILE_BUTTON_GAP;

    float max_scroll = std::max(0.0f, total_content_height - container_height + container_padding * 2);

    if (point_in_rectangle(mouse_position(), container_rect)) {
        float wheel = mouse_wheel_scroll().y;
        if (wheel != 0.0f) {
            scroll_target -= wheel * 30.0f;
            scroll_target = std::max(0.0f, std::min(scroll_target, max_scroll));
        }
    }

    scroll_offset += (scroll_target - scroll_offset) * 0.2f;

    fill_rectangle(rgba_color(230, 230, 230, 255), container_rect);

    float current_y = container_top + container_padding - scroll_offset;
    for (int i = 0; i < (int)(save_file_names.size()); i++) {
        const std::string& name = save_file_names[i].first;
        const std::string& desc = save_file_names[i].second;
        float row_height = row_height_for(save_file_names[i]);
        float button_y_position = current_y;

        if (button_y_position + row_height > container_top && button_y_position < container_bottom) {
            bool clicked = unique_button(name, rectangle_from(
                row_x,
                button_y_position,
                FILE_BUTTON_WIDTH,
                FILE_BUTTON_HEIGHT
            ));

            if (clicked) {
                sound_handler->play_sfx("ui_click");
                open_circuit(name);
                return;
            }

            if (!desc.empty()) {
                std::vector<std::string> desc_lines = wrap_text(desc, row_total_width, LOAD_FONT_STR, (int)FILE_DESC_FONT_SIZE);
                float desc_y = button_y_position + FILE_BUTTON_HEIGHT + FILE_ROW_DESC_PADDING;
                color desc_color = rgba_color(90, 90, 90, 255);
                for (const std::string& line : desc_lines) {
                    if (desc_y >= container_top && desc_y < container_bottom) {
                        draw_text(line, desc_color, LOAD_FONT_STR, (int)FILE_DESC_FONT_SIZE, row_x, desc_y);
                    }
                    desc_y += FILE_DESC_LINE_HEIGHT;
                }
            }

            float edit_x = row_x + FILE_BUTTON_WIDTH + DELETE_BUTTON_GAP;
            bool edit_clicked = unique_button("Edit", rectangle_from(
                edit_x,
                button_y_position,
                EDIT_BTN_WIDTH,
                FILE_BUTTON_HEIGHT
            ));

            if (edit_clicked) {
                sound_handler->play_sfx("ui_click");
                pending_edit_name = name;
                return;
            }

            float delete_x = edit_x + EDIT_BTN_WIDTH + DELETE_BUTTON_GAP;
            bool delete_clicked = unique_button("X", rectangle_from(
                delete_x,
                button_y_position,
                DELETE_BUTTON_WIDTH,
                FILE_BUTTON_HEIGHT
            ));

            if (delete_clicked) {
                sound_handler->play_sfx("ui_click");
                pending_delete_name = name;
                return;
            }
        }

        current_y += row_height + FILE_BUTTON_GAP;
    }

    if (!error_message.empty()) {
        float error_x = (window_handler->window_width / 2.0f) - (text_width(error_message, LOAD_FONT_STR, 16) / 2.0f);
        draw_text(error_message, COLOR_RED, LOAD_FONT_STR, 16, error_x, window_handler->window_height - 30.0f);
    }
}