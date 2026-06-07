#include "edit_circuit_details_menu.hpp"
#include "../../handlers/menu_handler.hpp"
#include "../../handlers/circuit_file_handler.hpp"
#include "../../handlers/sound_handler.hpp"
#include "../../utils/terminal_utils.h"
#include "../../utils/utilities.h"
#include "../components/BackButton.hpp"
#include "../components/UniqueButton.hpp"
#include "splashkit.h"
#include <format>

const std::string EDIT_FONT = "JetBrainsMono-Regular";
const float EDIT_FIELD_WIDTH = 460.0f;
const float EDIT_FIELD_HEIGHT = 44.0f;
const float EDIT_LABEL_FONT_SIZE = 16.0f;
const float EDIT_CONTENT_START_Y = 120.0f;
const float EDIT_FIELD_GAP = 48.0f;
const float EDIT_BUTTON_WIDTH = 120.0f;
const float EDIT_BUTTON_HEIGHT = 44.0f;
const float EDIT_BUTTON_GAP = 16.0f;
const int EDIT_MAX_DESC_LENGTH = 80;

EditCircuitDetailsMenu::EditCircuitDetailsMenu(
    std::string current_name,
    std::string current_description,
    std::string original_name,
    std::function<void(const std::string&, const std::string&)> on_save
)   : name_value(std::move(current_name)),
      description_value(std::move(current_description)),
      original_name(std::move(original_name)),
      on_save(std::move(on_save))
{
    print_info("EditCircuitDetailsMenu Initialized");
}

void EditCircuitDetailsMenu::handle_input() {
    if (pending_cancel) {
        pending_cancel = false;
        menu_handler->pop();
        return;
    }

    if (pending_save) {
        pending_save = false;

        if (!is_valid_circuit_name(name_value)) {
            error_message = "Name cannot be empty or contain: /, \\, :, *, ?, \", <, >, |";
            return;
        }

        if (description_value.length() > EDIT_MAX_DESC_LENGTH) {
            error_message = std::format("Maximum description length of {} characters", EDIT_MAX_DESC_LENGTH);
            return;
        }

        CircuitFileHandler file_handler;
        if (name_value != original_name && file_handler.save_exists(name_value)) {
            error_message = "A circuit named '" + name_value + "' already exists.";
            return;
        }

        on_save(name_value, description_value);
        sound_handler->play_sfx("click_success");
        menu_handler->pop();
        return;
    }

    float field_x = (window_handler->window_width / 2.0f) - (EDIT_FIELD_WIDTH / 2.0f);
    float current_y = EDIT_CONTENT_START_Y + EDIT_LABEL_FONT_SIZE + 6.0f;

    name_value = text_box(name_value, rectangle_from(field_x, current_y, EDIT_FIELD_WIDTH, EDIT_FIELD_HEIGHT));
    current_y += EDIT_FIELD_HEIGHT + EDIT_FIELD_GAP + EDIT_LABEL_FONT_SIZE + 6.0f;

    description_value = text_box(description_value, rectangle_from(field_x, current_y, EDIT_FIELD_WIDTH, EDIT_FIELD_HEIGHT));
}

void EditCircuitDetailsMenu::draw() const {
    bool clicked_back = draw_back_button();
    if (clicked_back) {
        sound_handler->play_sfx("ui_click");
        pending_cancel = true;
        return;
    }

    std::string title_text = "Edit Circuit Details";
    int title_font_size = 28;
    float title_x = (window_handler->window_width / 2.0f) - (text_width(title_text, EDIT_FONT, title_font_size) / 2.0f);
    draw_text(title_text, COLOR_BLACK, EDIT_FONT, title_font_size, title_x, 55.0f);

    float field_x = (window_handler->window_width / 2.0f) - (EDIT_FIELD_WIDTH / 2.0f);
    float current_y = EDIT_CONTENT_START_Y;

    draw_text("Name", COLOR_BLACK, EDIT_FONT, (int)EDIT_LABEL_FONT_SIZE, field_x, current_y);
    current_y += EDIT_LABEL_FONT_SIZE + 6.0f + EDIT_FIELD_HEIGHT + EDIT_FIELD_GAP;

    draw_text("Description", COLOR_BLACK, EDIT_FONT, (int)EDIT_LABEL_FONT_SIZE, field_x, current_y);
    current_y += EDIT_LABEL_FONT_SIZE + 6.0f + EDIT_FIELD_HEIGHT + EDIT_FIELD_GAP;

    float buttons_total_width = EDIT_BUTTON_WIDTH * 2 + EDIT_BUTTON_GAP;
    float buttons_x = (window_handler->window_width / 2.0f) - (buttons_total_width / 2.0f);

    bool save_clicked = unique_button("Save", rectangle_from(buttons_x, current_y, EDIT_BUTTON_WIDTH, EDIT_BUTTON_HEIGHT));
    if (save_clicked) {
        sound_handler->play_sfx("ui_click");
        pending_save = true;
    }

    bool cancel_clicked = unique_button("Cancel", rectangle_from(buttons_x + EDIT_BUTTON_WIDTH + EDIT_BUTTON_GAP, current_y, EDIT_BUTTON_WIDTH, EDIT_BUTTON_HEIGHT));
    if (cancel_clicked) {
        sound_handler->play_sfx("ui_click");
        pending_cancel = true;
    }

    if (!error_message.empty()) {
        float error_x = (window_handler->window_width / 2.0f) - (text_width(error_message, EDIT_FONT, 16) / 2.0f);
        draw_text(error_message, COLOR_RED, EDIT_FONT, 16, error_x, current_y + EDIT_BUTTON_HEIGHT + 16.0f);
    }
}