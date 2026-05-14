#include <iostream>
#include "splashkit.h"
#include "handlers/font_handler.hpp"
#include "handlers/menu_handler.hpp"
#include "handlers/settings_handler.hpp"
#include "handlers/sound_handler.hpp"
#include "utils/terminal_utils.h"
#include "handlers/window_handler.hpp"
#include "ui/menus/main_menu.hpp"

int main() {
    print_info("Initialising game");

    SettingsHandler game_settings;
    game_settings.load_settings();

    print_info("Initialising window");
    WindowHandler window_handler;
    window_handler.init_window(game_settings);

    print_info("Initialising fonts");
    FontHandler font_handler;
    int fonts_loaded = font_handler.init_fonts();
    if (fonts_loaded == 1) {
        return 1;
    }

    print_info("Initialising sound effects");
    SoundHandler sound_handler;
    int sounds_loaded = sound_handler.init_sfx();
    if (sounds_loaded == 1) {
        return 1;
    }

    print_info("Initialising menu system");
    MenuHandler menu_handler = MenuHandler(window_handler);
    menu_handler.push(std::make_unique<MainMenu>(window_handler.window_width, window_handler.window_height));

    print_info("Starting main game loop");
    while (!window_close_requested("Logic Circuit Simulator")) {
        process_events();
        menu_handler.update();
        draw_interface();
        refresh_screen(60);
    }
    print_info("Quit requested");

    return 0;
}