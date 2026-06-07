#include "settings_menu.hpp"
#include "../../handlers/menu_handler.hpp"
#include "../../handlers/sound_handler.hpp"
#include "../components/BackButton.hpp"
#include "../components/UniqueButton.hpp"
#include "splashkit.h"

const std::string SETTINGS_FONT = "JetBrainsMono-Regular";
const float SETTINGS_LABEL_FONT_SIZE = 20.0f;
const float SETTINGS_ROW_HEIGHT = 60.0f;
const float SETTINGS_CONTROL_WIDTH = 300.0f;
const float SETTINGS_CONTROL_HEIGHT = 30.0f;

const std::vector<std::pair<int, int>> RESOLUTIONS = {
    {1280, 720},
    {1280, 800},
    {1366, 768},
    {1440, 900},
    {1600, 900},
    {1680, 1050},
    {1920, 1080},
    {1920, 1200},
    {2560, 1440}
};

SettingsMenu::SettingsMenu(SettingsHandler& settings_handler)
    : settings_handler(settings_handler),
      bg_volume(0.0f),
      sfx_volume(0.0f),
      show_grid(true),
      snap_to_grid(true),
      selected_res_index(1),
      monitor_width(1920),
      monitor_height(1080)
{
    print_info("SettingsMenu Initialized");
}

void SettingsMenu::on_enter(WindowHandler& win_handler, MenuHandler& main_handler, SoundHandler& snd_handler) {
    Menu::on_enter(win_handler, main_handler, snd_handler);
    bg_volume = (float)(settings_handler.get_setting<double>("bgVolume"));
    sfx_volume = (float)(settings_handler.get_setting<double>("sfxVolume"));
    show_grid = settings_handler.get_setting<bool>("showGrid");
    snap_to_grid = settings_handler.get_setting<bool>("snapToGrid");

    int saved_width = settings_handler.get_setting<int>("screenWidth");
    int saved_height = settings_handler.get_setting<int>("screenHeight");

    display primary_disp = display_details(0);
    monitor_width = display_width(primary_disp);
    monitor_height = display_height(primary_disp);

    selected_res_index = 1;
    for (int i = 0; i < (int)RESOLUTIONS.size(); i++) {
        if (RESOLUTIONS[i].first == saved_width && RESOLUTIONS[i].second == saved_height) {
            selected_res_index = i;
            break;
        }
    }
}

void SettingsMenu::apply_settings() const {
    settings_handler.set_setting("bgVolume", (double)(bg_volume));
    settings_handler.set_setting("sfxVolume", (double)(sfx_volume));
    settings_handler.set_setting("showGrid", show_grid);
    settings_handler.set_setting("snapToGrid", snap_to_grid);
    settings_handler.set_setting("screenWidth", RESOLUTIONS[selected_res_index].first);
    settings_handler.set_setting("screenHeight", RESOLUTIONS[selected_res_index].second);
    settings_handler.save_settings();

    sound_handler->set_bg_volume((double)(bg_volume));
    sound_handler->set_sfx_volume((double)sfx_volume);

    window_handler->window_width = RESOLUTIONS[selected_res_index].first;
    window_handler->window_height = RESOLUTIONS[selected_res_index].second;
    resize_current_window(window_handler->window_width, window_handler->window_height);

    print_info("Settings applied");
}

void SettingsMenu::handle_input() {}

void SettingsMenu::draw() const {
    bool clicked_back = draw_back_button();
    if (clicked_back) {
        sound_handler->play_sfx("ui_click");
        menu_handler->pop();
        return;
    }

    std::string title_text = "Settings";

    int title_font_size = 36;
    float title_x = (window_handler->window_width / 2.0f) - (text_width(title_text, SETTINGS_FONT, title_font_size) / 2.0f);
    draw_text(title_text, COLOR_BLACK, SETTINGS_FONT, title_font_size, title_x, 60.0f);

    float label_x = (window_handler->window_width / 2.0f) - 260.0f;
    float control_x = (window_handler->window_width / 2.0f) + 60.0f;
    float row_y = 160.0f;

    draw_text("Background Volume", COLOR_BLACK, SETTINGS_FONT, (int)(SETTINGS_LABEL_FONT_SIZE), label_x, row_y + 5.0f);
    bg_volume = slider(bg_volume, 0.0f, 1.0f, rectangle_from(control_x, row_y, SETTINGS_CONTROL_WIDTH, SETTINGS_CONTROL_HEIGHT));
    row_y += SETTINGS_ROW_HEIGHT;

    draw_text("SFX Volume", COLOR_BLACK, SETTINGS_FONT, (int)(SETTINGS_LABEL_FONT_SIZE), label_x, row_y + 5.0f);
    sfx_volume = slider(sfx_volume, 0.0f, 1.0f, rectangle_from(control_x, row_y, SETTINGS_CONTROL_WIDTH, SETTINGS_CONTROL_HEIGHT));
    row_y += SETTINGS_ROW_HEIGHT;

    draw_text("Show Grid", COLOR_BLACK, SETTINGS_FONT, (int)(SETTINGS_LABEL_FONT_SIZE), label_x, row_y + 5.0f);
    show_grid = checkbox("", show_grid, rectangle_from(control_x, row_y, SETTINGS_CONTROL_HEIGHT, SETTINGS_CONTROL_HEIGHT));
    if (!show_grid) {
        snap_to_grid = false;
    }
    row_y += SETTINGS_ROW_HEIGHT;

    draw_text("Snap To Grid", COLOR_BLACK, SETTINGS_FONT, (int)(SETTINGS_LABEL_FONT_SIZE), label_x, row_y + 5.0f);
    if (show_grid) {
        snap_to_grid = checkbox("", snap_to_grid, rectangle_from(control_x, row_y, SETTINGS_CONTROL_HEIGHT, SETTINGS_CONTROL_HEIGHT));
    } else {
        draw_text("(requires Show Grid)", rgba_color(160, 160, 160, 255), SETTINGS_FONT, 14, control_x + SETTINGS_CONTROL_HEIGHT + 12.0f, row_y + 7.0f);
        checkbox("", false, rectangle_from(control_x, row_y, SETTINGS_CONTROL_HEIGHT, SETTINGS_CONTROL_HEIGHT));
    }
    row_y += SETTINGS_ROW_HEIGHT;

    int max_res_index = 0;
    for (int i = (int)RESOLUTIONS.size() - 1; i >= 0; i--) {
        if (RESOLUTIONS[i].first <= monitor_width && RESOLUTIONS[i].second <= monitor_height) {
            max_res_index = i;
            break;
        }
    }
    if (selected_res_index > max_res_index) {
        selected_res_index = max_res_index;
    }

    std::string resolution_label = std::format("{}x{}", RESOLUTIONS[selected_res_index].first, RESOLUTIONS[selected_res_index].second);
    float arrow_width = 30.0f;
    float resolution_label_width = SETTINGS_CONTROL_WIDTH - arrow_width * 2;

    draw_text("Resolution", COLOR_BLACK, SETTINGS_FONT, (int)(SETTINGS_LABEL_FONT_SIZE), label_x, row_y + 5.0f);

    bool left_btn_clicked = unique_button("<", rectangle_from(control_x, row_y, arrow_width, SETTINGS_CONTROL_HEIGHT));
    bool right_btn_clicked = unique_button(">", rectangle_from(control_x + arrow_width + resolution_label_width, row_y, arrow_width, SETTINGS_CONTROL_HEIGHT));

    draw_text(
        resolution_label,
        COLOR_BLACK,
        SETTINGS_FONT,
        (int)(SETTINGS_LABEL_FONT_SIZE),
        control_x + arrow_width + (resolution_label_width / 2.0f) - (text_width(resolution_label, SETTINGS_FONT, (int)(SETTINGS_LABEL_FONT_SIZE)) / 2.0f),
        row_y + 5.0f
    );

    if (left_btn_clicked) {
        if (selected_res_index > 0) {
            selected_res_index--;
        }
    }
    if (right_btn_clicked) {
        if (selected_res_index < max_res_index) {
            selected_res_index++;
        }
    }
    row_y += SETTINGS_ROW_HEIGHT;

    float apply_button_width = 160.0f;
    float apply_button_height = 44.0f;
    float apply_x = (window_handler->window_width / 2.0f) - (apply_button_width / 2.0f);

    bool apply_clicked = unique_button("Apply", rectangle_from(apply_x, row_y, apply_button_width, apply_button_height));
    if (apply_clicked) {
        apply_settings();
        sound_handler->play_sfx("ui_click");
    }
}