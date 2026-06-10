#include "splashkit.h"
#include "handlers/font_handler.hpp"
#include "handlers/menu_handler.hpp"
#include "handlers/settings_handler.hpp"
#include "handlers/sound_handler.hpp"
#include "handlers/window_handler.hpp"
#include "ui/menus/main_menu.hpp"
#include "utils/terminal_utils.h"

#ifdef __APPLE__
    #include <mach-o/dyld.h>
#endif

int main() {
    // --- Initialisation ------------------------------------------------------
    #ifdef __APPLE__
        char path[PATH_MAX];
        uint32_t size = PATH_MAX;
        _NSGetExecutablePath(path, &size);
        std::filesystem::current_path(std::filesystem::path(path).parent_path());
    #endif

    print_info(std::format("LCS-{}", std::string(APP_VERSION)));
    print_info("Initialising settings");
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

    // --- Audio ---------------------------------------------------------------
    double bg_volume  = game_settings.get_setting<double>("bgVolume");
    double sfx_volume = game_settings.get_setting<double>("sfxVolume");
    print_info(std::format("Background Volume: {:.2f}", bg_volume));
    print_info(std::format("SFX Volume: {:.2f}", sfx_volume));

    sound_handler.set_bg_volume(bg_volume);
    sound_handler.set_sfx_volume(sfx_volume);

    print_info("Starting background music");
    music background_music = music_named("background_track1");
    play_music(background_music, 10000, (float)(bg_volume));

    // --- Main loop -----------------------------------------------------------
    print_info("Entering main loop");
    while (!window_close_requested(window_handler.WINDOW_NAME)) {
        process_events();

        set_interface_font_size(24);
        set_interface_font("JetBrainsMono-Regular");
        menu_handler.update();
        if (window_handler.window_closed) {
            break;
        }
        draw_interface();
        draw_text(std::format("LCS-{}", std::string(APP_VERSION)), COLOR_GRAY, "JetBrainsMono-Regular", 16, 10.0f, window_handler.window_height - 30.0f);

        refresh_screen(60);
    }

    // --- Cleanup -------------------------------------------------------------

    print_info("Freeing music");
    sound_handler.free_music_objects();
    print_info("Cleanup complete");

    return 0;
}
