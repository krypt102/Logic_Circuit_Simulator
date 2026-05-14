#include <iostream>
#include "splashkit.h"
#include "handlers/settings_handler.hpp"
#include "utils/terminal_utils.h"
#include "handlers/ui_handler.hpp"

int main() {
    print_info("Initialising game");

    SettingsHandler game_settings;
    game_settings.load_settings();

    print_info("Initialising window");
    UiHandler ui_handler;
    ui_handler.init_window(game_settings);

    print_info("Initialising menu system");
    delay(3000);

    return 0;
}
