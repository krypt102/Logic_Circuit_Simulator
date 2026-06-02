#include "Modal.hpp"
#include "../../handlers/menu_handler.hpp"
#include "splashkit.h"

const float MODAL_WIDTH = 480.0f;
const float MODAL_PADDING = 32.0f;
const float MODAL_BUTTON_HEIGHT = 44.0f;
const float MODAL_BUTTON_GAP = 12.0f;
const float MODAL_TITLE_FONT_SIZE = 24.0f;
const float MODAL_MESSAGE_FONT_SIZE = 18.0f;
const float MODAL_BUTTON_FONT_SIZE = 18.0f;
const std::string MODAL_FONT = "JetBrainsMono-Regular";

const color MODAL_COLOR_OVERLAY = rgba_color(0, 0, 0, 160);
const color MODAL_COLOR_PANEL = rgba_color(245, 245, 245, 255);
const color MODAL_COLOR_PANEL_OUTLINE = rgba_color(180, 180, 180, 255);
const color MODAL_COLOR_TITLE = rgba_color(30, 30, 30, 255);
const color MODAL_COLOR_MESSAGE = rgba_color(60, 60, 60, 255);

Modal::Modal(std::string title, std::string message, std::vector<ModalButton> buttons)
    : title(std::move(title)),
      message(std::move(message)),
      buttons(std::move(buttons))
{
    is_overlay = true;
}

void Modal::on_enter(WindowHandler& win_handler, MenuHandler& main_handler) {
    window_handler = &win_handler;
    menu_handler = &main_handler;
    window_width = win_handler.window_width;
    window_height = win_handler.window_height;
}

void Modal::handle_input() {}

void Modal::draw() const {
    fill_rectangle(MODAL_COLOR_OVERLAY, 0, 0, window_width, window_height);

    float buttons_total_height =
        (int)(buttons.size()) * MODAL_BUTTON_HEIGHT +
        ((int)(buttons.size()) - 1) * MODAL_BUTTON_GAP;

    float panel_height =
        MODAL_PADDING +
        MODAL_TITLE_FONT_SIZE +
        MODAL_PADDING +
        MODAL_MESSAGE_FONT_SIZE +
        MODAL_PADDING +
        buttons_total_height +
        MODAL_PADDING;

    float panel_x = (window_width / 2.0f) - (MODAL_WIDTH / 2.0f);
    float panel_y = (window_height / 2.0f) - (panel_height / 2.0f);

    fill_rectangle(MODAL_COLOR_PANEL, panel_x, panel_y, MODAL_WIDTH, panel_height);
    draw_rectangle(MODAL_COLOR_PANEL_OUTLINE, panel_x, panel_y, MODAL_WIDTH, panel_height);

    float title_x = panel_x + (MODAL_WIDTH / 2.0f) - (text_width(title, MODAL_FONT, (int)(MODAL_TITLE_FONT_SIZE)) / 2.0f);
    float title_y = panel_y + MODAL_PADDING;
    draw_text(title, MODAL_COLOR_TITLE, MODAL_FONT, (int)(MODAL_TITLE_FONT_SIZE), title_x, title_y);

    float message_x = panel_x + (MODAL_WIDTH / 2.0f) - (text_width(message, MODAL_FONT, (int)(MODAL_MESSAGE_FONT_SIZE)) / 2.0f);
    float message_y = title_y + MODAL_TITLE_FONT_SIZE + MODAL_PADDING;
    draw_text(message, MODAL_COLOR_MESSAGE, MODAL_FONT, (int)(MODAL_MESSAGE_FONT_SIZE), message_x, message_y);

    float button_width = (MODAL_WIDTH - MODAL_PADDING * 2 - MODAL_BUTTON_GAP * ((int)(buttons.size()) - 1)) / (int)(buttons.size());
    float buttons_y = message_y + MODAL_MESSAGE_FONT_SIZE + MODAL_PADDING;
    float button_x = panel_x + MODAL_PADDING;

    for (int i = 0; i < (int)(buttons.size()); i++) {
        rectangle btn_rect = rectangle_from(button_x, buttons_y, button_width, MODAL_BUTTON_HEIGHT);
        bool clicked = button(buttons[i].label, btn_rect);
        if (clicked) {
            buttons[i].on_click();
        }
        button_x += button_width + MODAL_BUTTON_GAP;
    }
}