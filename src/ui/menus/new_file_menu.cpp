#include "new_file_menu.hpp"
#include "../../handlers/menu_handler.hpp"
#include "splashkit.h"
#include "../components/BackButton.hpp"

const float FORM_WIDTH = 440.0f;
const float FORM_HEIGHT = 40.0f;
const float VERTICAL_GAP = 56.0f;

const std::string FONT_STR = "JetBrainsMono-Regular";

NewFileMenu::NewFileMenu(int window_width, int window_height)
    : window_width(window_width), window_height(window_height) {}

void NewFileMenu::on_enter(WindowHandler& win_handler, MenuHandler& main_handler) {
    window_handler = &win_handler;
    menu_handler = &main_handler;
    error_message.clear();
}

void NewFileMenu::handle_input() {
    float caption_text_size = 20;

    float name_box_x_position = (window_width / 2.0f) - (FORM_WIDTH / 2.0f);
    float name_box_y_position = 140.0f;
    std::string name_box_caption = "New file name:";

    float name_box_caption_x_position = name_box_x_position - text_width(name_box_caption, FONT_STR, caption_text_size) - 10.0f;
    float name_box_caption_y_position = name_box_y_position + (FORM_HEIGHT / 2.0f / 2.0f);
    draw_text(name_box_caption, COLOR_BLACK, FONT_STR, caption_text_size, name_box_caption_x_position, name_box_caption_y_position);

    float desc_box_x_position = (window_width / 2.0f) - (FORM_WIDTH / 2.0f);
    float desc_box_y_position = name_box_y_position + VERTICAL_GAP;
    std::string desc_box_caption = "New file description:";

    float desc_box_caption_x_position = desc_box_x_position - text_width(desc_box_caption, FONT_STR, caption_text_size) - 10.0f;
    float desc_box_caption_y_position = desc_box_y_position + (FORM_HEIGHT / 2.0f / 2.0f);
    draw_text(desc_box_caption, COLOR_BLACK, FONT_STR, caption_text_size, desc_box_caption_x_position, desc_box_caption_y_position);

    set_interface_font_size(caption_text_size);
    new_file_name = text_box(new_file_name, rectangle_from(name_box_x_position, name_box_y_position, FORM_WIDTH, FORM_HEIGHT));
    new_file_desc = text_box(new_file_desc, rectangle_from(desc_box_x_position, desc_box_y_position, FORM_WIDTH, FORM_HEIGHT));

    last_text_box_x_position = desc_box_x_position;
    last_text_box_y_position = desc_box_y_position;
}

void NewFileMenu::draw() const {
    bool has_clicked_back = draw_back_button();
    if (has_clicked_back) {
        play_sound_effect("ui_click");
        menu_handler->pop();
    }

    std::string title_text = "Create New File";
    int title_font_size = 28;
    float title_x_position = (window_width / 2.0f) - (text_width(title_text, FONT_STR, title_font_size) / 2.0f);
    draw_text(title_text, COLOR_BLACK, FONT_STR, title_font_size, title_x_position, 60.0f);

    bool create_button_pressed = button("Create file", rectangle_from(
        last_text_box_x_position + (FORM_WIDTH / 2.0f / 2.0f),
        last_text_box_y_position + VERTICAL_GAP,
        FORM_WIDTH / 2.0f,
        FORM_HEIGHT
    ));

    if (create_button_pressed) {
        play_sound_effect("ui_click");
        print_warning("Clicked create");
    }
    if (!error_message.empty()) {
        float error_x_position = (window_width / 2.0f) - (text_width(error_message, FONT_STR, 16) / 2.0f);
        draw_text(error_message, COLOR_RED, FONT_STR, 16, error_x_position, window_height - 60.0f);
    }
}