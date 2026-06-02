#include "settings_menu.hpp"
#include "../../handlers/menu_handler.hpp"
#include "../components/BackButton.hpp"
#include "splashkit.h"

const std::string SETTINGS_FONT = "JetBrainsMono-Regular";
const float SETTINGS_LABEL_FONT_SIZE = 20.0f;
const float SETTINGS_ROW_HEIGHT = 60.0f;
const float SETTINGS_CONTROL_WIDTH = 300.0f;
const float SETTINGS_CONTROL_HEIGHT = 30.0f;

SettingsMenu::SettingsMenu(int window_width, int window_height, SettingsHandler& settings_handler, SoundHandler& sound_handler)
    : window_width(window_width),
      window_height(window_height),
      settings_handler(settings_handler),
      sound_handler(sound_handler),
      bg_volume(0.0f),
      sfx_volume(0.0f),
      show_grid(true)
{}

void SettingsMenu::on_enter(WindowHandler& win_handler, MenuHandler& main_handler) {
    window_handler = &win_handler;
    menu_handler = &main_handler;

    bg_volume = (float)(settings_handler.get_setting<double>("bgVolume"));
    sfx_volume = (float)(settings_handler.get_setting<double>("sfxVolume"));
    show_grid = settings_handler.get_setting<bool>("showGrid");
}

void SettingsMenu::apply_settings() const {
    settings_handler.set_setting("bgVolume", (double)(bg_volume));
    settings_handler.set_setting("sfxVolume", (double)(sfx_volume));
    settings_handler.set_setting("showGrid", show_grid);
    settings_handler.save_settings();

    // Implement these soon
    // sound_handler.set_bg_volume((double)(bg_volume));
    // sound_handler.set_sfx_volume((double)(sfx_volume));

    print_info("Settings applied");
}

void SettingsMenu::handle_input() {}

void SettingsMenu::draw() const {
    bool clicked_back = draw_back_button();
    if (clicked_back) {
        play_sound_effect("ui_click");
        menu_handler->pop();
        return;
    }

    std::string title_text = "Settings";

    int title_font_size = 36;
    float title_x = (window_width / 2.0f) - (text_width(title_text, SETTINGS_FONT, title_font_size) / 2.0f);
    draw_text(title_text, COLOR_BLACK, SETTINGS_FONT, title_font_size, title_x, 60.0f);

    float label_x = (window_width / 2.0f) - 220.0f;
    float control_x = (window_width / 2.0f) - 20.0f;
    float row_y = 160.0f;

    set_interface_font(SETTINGS_FONT);
    set_interface_font_size(static_cast<int>(SETTINGS_LABEL_FONT_SIZE));

    draw_text("Background Volume", COLOR_BLACK, SETTINGS_FONT, static_cast<int>(SETTINGS_LABEL_FONT_SIZE), label_x, row_y + 5.0f);
    bg_volume = slider(bg_volume, 0.0f, 1.0f, rectangle_from(control_x, row_y, SETTINGS_CONTROL_WIDTH, SETTINGS_CONTROL_HEIGHT));
    row_y += SETTINGS_ROW_HEIGHT;

    draw_text("SFX Volume", COLOR_BLACK, SETTINGS_FONT, static_cast<int>(SETTINGS_LABEL_FONT_SIZE), label_x, row_y + 5.0f);
    sfx_volume = slider(sfx_volume, 0.0f, 1.0f, rectangle_from(control_x, row_y, SETTINGS_CONTROL_WIDTH, SETTINGS_CONTROL_HEIGHT));
    row_y += SETTINGS_ROW_HEIGHT;

    draw_text("Show Grid", COLOR_BLACK, SETTINGS_FONT, static_cast<int>(SETTINGS_LABEL_FONT_SIZE), label_x, row_y + 5.0f);
    show_grid = checkbox("", show_grid, rectangle_from(control_x, row_y, SETTINGS_CONTROL_HEIGHT, SETTINGS_CONTROL_HEIGHT));
    row_y += SETTINGS_ROW_HEIGHT;

    draw_text("Screen resolution changes require a restart.", rgba_color(120, 120, 120, 255), SETTINGS_FONT, 14, label_x, row_y);
    row_y += 36.0f;

    std::string res_text = std::to_string(window_handler->window_width) + " x " + std::to_string(window_handler->window_height);
    draw_text(res_text, COLOR_BLACK, SETTINGS_FONT, static_cast<int>(SETTINGS_LABEL_FONT_SIZE), label_x, row_y);
    row_y += SETTINGS_ROW_HEIGHT;

    float apply_button_width = 160.0f;
    float apply_button_height = 44.0f;
    float apply_x = (window_width / 2.0f) - (apply_button_width / 2.0f);

    bool apply_clicked = button("Apply", rectangle_from(apply_x, row_y, apply_button_width, apply_button_height));
    if (apply_clicked) {
        play_sound_effect("ui_click");
        apply_settings();
    }
}