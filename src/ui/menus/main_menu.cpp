#include "main_menu.hpp"

#include "load_file_menu.hpp"
#include "new_file_menu.hpp"
#include "settings_menu.hpp"
#include "splashkit.h"
#include "../../handlers/menu_handler.hpp"
#include "../../handlers/window_handler.hpp"
#include "../components/UniqueButton.hpp"
#include "../components/Modal.hpp"

const float MAIN_MENU_BUTTON_WIDTH = 280.0f;
const float MAIN_MENU_BUTTON_HEIGHT = 52.0f;
const float MAIN_MENU_BUTTON_GAP = 20.0f;

const std::string MAIN_MENU_FONT = "JetBrainsMono-Regular";

MainMenu::MainMenu(SettingsHandler& settings_handler)
    : settings_handler(settings_handler)
{
    print_info("MainMenu Initialized");
}

void MainMenu::handle_input() {}

void MainMenu::draw() const {
    const std::string title_text = "Logic Circuit Simulator";
    const int title_font_size = 42;
    float title_x = (window_handler->window_width / 2.0f) - (text_width(title_text, MAIN_MENU_FONT, title_font_size) / 2.0f);
    draw_text(title_text, COLOR_BLACK, MAIN_MENU_FONT, title_font_size, title_x, 80.0f);
    
    float group_height = (4 * MAIN_MENU_BUTTON_HEIGHT) + (3 * MAIN_MENU_BUTTON_GAP);
    float centre_x = (window_handler->window_width  / 2.0f) - (MAIN_MENU_BUTTON_WIDTH / 2.0f);
    float start_y = (window_handler->window_height / 2.0f) - (group_height / 2.0f);

    auto button_rect = [&](int index) {
        return rectangle_from(
            centre_x,
            start_y + index * (MAIN_MENU_BUTTON_HEIGHT + MAIN_MENU_BUTTON_GAP),
            MAIN_MENU_BUTTON_WIDTH,
            MAIN_MENU_BUTTON_HEIGHT
        );
    };

    bool new_file_clicked = unique_button("New File", button_rect(0));
    bool load_file_clicked = unique_button("Load File", button_rect(1));
    bool settings_clicked = unique_button("Settings", button_rect(2));
    bool quit_clicked = unique_button("Quit", button_rect(3));

    if (new_file_clicked) {
        sound_handler->play_sfx("ui_click");
        menu_handler->push(std::make_unique<NewFileMenu>(settings_handler));
    } else if (load_file_clicked) {
        sound_handler->play_sfx("ui_click");
        menu_handler->push(std::make_unique<LoadFileMenu>(settings_handler));
    } else if (settings_clicked) {
        sound_handler->play_sfx("ui_click");
        menu_handler->push(std::make_unique<SettingsMenu>(settings_handler));
    } else if (quit_clicked) {
        sound_handler->play_sfx("ui_click");
        print_info("User requested quit");
        std::vector<ModalButton> quit_buttons;

        quit_buttons.push_back({"Yes", [this]() {
            window_handler->close_game_window();
        }});

        quit_buttons.push_back({"No", [this]() {
            menu_handler->pop();
        }});

        menu_handler->push(std::make_unique<Modal>(
            "Are you sure?",
            "Are you sure you want to quit?",
            std::move(quit_buttons)
        ));
    }
}
