#include "menu_handler.hpp"
#include "splashkit.h"

void MenuHandler::push(std::unique_ptr<Menu> menu) {
    Menu* raw = menu.get();
    menus.push_back(std::move(menu));
    raw->on_enter(window_handler, *this);
}

void MenuHandler::pop() {
    // We want one menu on the screen at all times
    if (menus.size() <= 1) {
        return;
    }

    menus.pop_back();
    current_menu()->on_enter(window_handler, *this);
}

void MenuHandler::update() const {
    Menu* menu = current_menu();
    if (!menu) {
        return;
    }

    clear_screen(COLOR_WHITE);
    menu->handle_input();
    menu->draw();
}

Menu* MenuHandler::current_menu() const {
    if (menus.empty()) {
        return nullptr;
    }

    return menus.back().get();
}