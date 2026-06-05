#include "splashkit.h"
#include "handlers/font_handler.hpp"
#include "handlers/menu_handler.hpp"
#include "handlers/settings_handler.hpp"
#include "handlers/sound_handler.hpp"
#include "handlers/window_handler.hpp"
#include "ui/menus/main_menu.hpp"
#include "utils/terminal_utils.h"

int main() {
    print_info("Initialising game");

    SettingsHandler game_settings;
    game_settings.load_settings();

    print_info("Initialising fonts");
    FontHandler font_handler;
    if (font_handler.init_fonts() == 1) {
        return 1;
    }

    print_info("Initialising sound effects");
    SoundHandler sound_handler;
    if (sound_handler.init_sfx() == 1) {
        return 1;
    }
    if (sound_handler.init_music_objects() == 1) {
        return 1;
    }

    print_info("Initialising window");
    WindowHandler window_handler;
    window_handler.init_window(game_settings);

    print_info("Initialising menu system");
    MenuHandler menu_handler(window_handler, sound_handler);
    menu_handler.push(std::make_unique<MainMenu>(game_settings));

    print_info("Starting main game loop");
    double bg_volume = game_settings.get_setting<double>("bgVolume");
    print_info("Background volume: " + std::to_string(bg_volume));
    sound_handler.set_bg_volume(bg_volume);

    double sfx_volume = game_settings.get_setting<double>("sfxVolume");
    print_info("Sound effects volume: " + std::to_string(sfx_volume));
    sound_handler.set_sfx_volume(sfx_volume);

    print_info("Starting music");
    music background_music = music_named("background_track1");
    play_music(background_music, 10000, bg_volume);
    while (!window_close_requested("Logic Circuit Simulator")) {
        process_events();

        set_interface_font_size(24);
        set_interface_font("JetBrainsMono-Regular");
        menu_handler.update();
        draw_interface();
        refresh_screen(60);
    }
    print_info("Freeing music");
    sound_handler.free_music_objects();
    print_info("Quit requested");

    return 0;
}