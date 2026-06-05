#include "Menu.hpp"

void Menu::on_enter(WindowHandler& win_handler, MenuHandler& main_handler, SoundHandler& snd_handler) {
    window_handler = &win_handler;
    menu_handler = &main_handler;
    sound_handler = &snd_handler;
}