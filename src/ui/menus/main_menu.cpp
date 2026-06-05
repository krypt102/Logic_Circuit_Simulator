#include "main_menu.hpp"

#include "load_file_menu.hpp"
#include "new_file_menu.hpp"
#include "settings_menu.hpp"
#include "splashkit.h"
#include "../../handlers/menu_handler.hpp"
#include "../../handlers/window_handler.hpp"
#include "../components/UniqueButton.hpp"

const float BUTTON_WIDTH = 280.0f;
const float BUTTON_HEIGHT = 52.0f;
const float BUTTON_GAP = 20.0f;

MainMenu::MainMenu(SettingsHandler& settings_handler)
    : settings_handler(settings_handler)
{
    print_info("Initialising main menu");
}

void MainMenu::handle_input() {}

void MainMenu::draw() const {
    string title_text = "Logic Circuit Simulator";
    string regular_font_name = "JetBrainsMono-Regular";

    int title_font_size = 42;

    float title_x = (window_handler->window_width / 2.0f) - (text_width(title_text, regular_font_name, title_font_size) / 2.0f);
    draw_text(title_text, COLOR_BLACK, regular_font_name, title_font_size, title_x, 80.0f);

    float button_group_height = (3 * BUTTON_HEIGHT) + (2 * BUTTON_GAP);
    float centre_x_position = (window_handler->window_width / 2.0f) - (BUTTON_WIDTH / 2.0f);
    float start_y_position = (window_handler->window_height / 2.0f) - (button_group_height / 2.0f);

    bool new_file_clicked = unique_button("New File", rectangle_from(
        centre_x_position,
        start_y_position,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    ));

    bool load_file_clicked = unique_button("Load File", rectangle_from(
        centre_x_position,
        start_y_position + BUTTON_HEIGHT + BUTTON_GAP,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    ));

    bool settings_btn_clicked = unique_button("Settings", rectangle_from(
        centre_x_position,
        start_y_position + (2 * (BUTTON_HEIGHT + BUTTON_GAP)),
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    ));

    bool quit_clicked = unique_button("Quit", rectangle_from(
        centre_x_position,
        start_y_position + (3 * (BUTTON_HEIGHT + BUTTON_GAP)),
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    ));

    if (new_file_clicked) {
        sound_handler->play_sfx("ui_click");
        menu_handler->push(std::make_unique<NewFileMenu>(settings_handler));
    } else if (load_file_clicked) {
        sound_handler->play_sfx("ui_click");
        menu_handler->push(std::make_unique<LoadFileMenu>(settings_handler));
    } else if (settings_btn_clicked) {
        sound_handler->play_sfx("ui_click");
        menu_handler->push(std::make_unique<SettingsMenu>(settings_handler));
    } else if (quit_clicked) {
        sound_handler->play_sfx("ui_click");
        print_info("Quit clicked");
    }
}