#include "new_file_menu.hpp"

#include <format>

#include "circuit_editor_menu.hpp"
#include "../../handlers/menu_handler.hpp"
#include "../../handlers/circuit_file_handler.hpp"
#include "splashkit.h"
#include "../components/BackButton.hpp"
#include "../components/UniqueButton.hpp"
#include "../../utils/utilities.h"

const float NEW_FILE_MENU_FORM_WIDTH   = 440.0f;
const float NEW_FILE_MENU_FORM_HEIGHT  = 40.0f;
const float NEW_FILE_MENU_VERTICAL_GAP = 56.0f;
const int NEW_FILE_MENU_MAX_DESC_LEN = 80;

const std::string NEW_FILE_MENU_FONT = "JetBrainsMono-Regular";

NewFileMenu::NewFileMenu(SettingsHandler& settings_handler)
    : settings_handler(settings_handler)
{
    print_info("NewFileMenu Initialized");
}

void NewFileMenu::on_enter(WindowHandler& win_handler, MenuHandler& main_handler, SoundHandler& snd_handler) {
    Menu::on_enter(win_handler, main_handler, snd_handler);
    error_message.clear();
}

void NewFileMenu::try_create_circuit() const {
    if (!is_valid_circuit_name(circuit_name)) {
        error_message = "Name cannot be empty or contain: /, \\, :, *, ?, \", <, >, |";
        return;
    }

    if (circuit_description.length() > NEW_FILE_MENU_MAX_DESC_LEN) {
        error_message = std::format("Maximum description length of {} characters", NEW_FILE_MENU_MAX_DESC_LEN);
        return;
    }

    CircuitFileHandler file_handler;
    if (file_handler.save_exists(circuit_name)) {
        error_message = std::format(R"(A circuit named "{}" already exists)", circuit_name);
        return;
    }

    Circuit new_circuit(circuit_name, circuit_description);
    if (!file_handler.save_circuit(new_circuit)) {
        error_message = "Failed to save circuit. Check the saves folder.";
        return;
    }

    print_info(std::format("Created new circuit: {}", circuit_name));
    circuit_name.clear();
    circuit_description.clear();
    error_message.clear();
    menu_handler->push(
        std::make_unique<CircuitEditorMenu>(std::move(new_circuit), settings_handler)
    );
}

void NewFileMenu::handle_input() {
    const float caption_font_size = 20.0f;
    float field_x = (window_handler->window_width / 2.0f) - (NEW_FILE_MENU_FORM_WIDTH / 2.0f);
    float name_y = 140.0f;
    float desc_y = name_y + NEW_FILE_MENU_VERTICAL_GAP;

    const std::string name_caption = "New file name:";
    draw_text(
        name_caption, COLOR_BLACK, NEW_FILE_MENU_FONT, (int)caption_font_size,
        field_x - text_width(name_caption, NEW_FILE_MENU_FONT, (int)caption_font_size) - 10.0f,
        name_y + (NEW_FILE_MENU_FORM_HEIGHT / 4.0f)
    );

    const std::string desc_caption = "New file description:";
    draw_text(
        desc_caption, COLOR_BLACK, NEW_FILE_MENU_FONT, (int)caption_font_size,
        field_x - text_width(desc_caption, NEW_FILE_MENU_FONT, (int)caption_font_size) - 10.0f,
        desc_y + (NEW_FILE_MENU_FORM_HEIGHT / 4.0f)
    );

    circuit_name = text_box(circuit_name, rectangle_from(field_x, name_y, NEW_FILE_MENU_FORM_WIDTH, NEW_FILE_MENU_FORM_HEIGHT));
    circuit_description = text_box(circuit_description, rectangle_from(field_x, desc_y, NEW_FILE_MENU_FORM_WIDTH, NEW_FILE_MENU_FORM_HEIGHT));
    last_field_x = field_x;
    last_field_y = desc_y;

    bool clicked_back = draw_back_button();
    if (clicked_back) {
        sound_handler->play_sfx("ui_click");
        menu_handler->pop();
        return;
    }
}

void NewFileMenu::draw() const {
    const std::string title_text = "Create New File";
    const int title_font_size = 28;
    float title_x = (window_handler->window_width / 2.0f) - (text_width(title_text, NEW_FILE_MENU_FONT, title_font_size) / 2.0f);
    draw_text(title_text, COLOR_BLACK, NEW_FILE_MENU_FONT, title_font_size, title_x, 60.0f);

    bool create_clicked = unique_button("Create file", rectangle_from(
        last_field_x + (NEW_FILE_MENU_FORM_WIDTH / 4.0f),
        last_field_y + NEW_FILE_MENU_VERTICAL_GAP,
        NEW_FILE_MENU_FORM_WIDTH / 2.0f,
        NEW_FILE_MENU_FORM_HEIGHT
    ));

    if (create_clicked) {
        sound_handler->play_sfx("ui_click");
        try_create_circuit();
    }

    if (!error_message.empty()) {
        float error_x = (window_handler->window_width / 2.0f) - (text_width(error_message, NEW_FILE_MENU_FONT, 16) / 2.0f);
        draw_text(error_message, COLOR_RED, NEW_FILE_MENU_FONT, 16, error_x, window_handler->window_height - 60.0f);
    }
}