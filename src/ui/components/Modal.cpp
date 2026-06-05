#include "Modal.hpp"
#include "../../handlers/menu_handler.hpp"
#include "../../utils/utilities.h"
#include "splashkit.h"
#include "UniqueButton.hpp"

const float MODAL_MIN_WIDTH = 480.0f;
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
{}

void Modal::on_enter(WindowHandler& win_handler, MenuHandler& main_handler) {
    window_handler = &win_handler;
    menu_handler = &main_handler;
    window_width = win_handler.window_width;
    window_height = win_handler.window_height;
}

void Modal::handle_input() {}

void Modal::draw() const {
    fill_rectangle(MODAL_COLOR_OVERLAY, 0, 0, window_width, window_height);

    std::vector<std::string> lines = split_lines(message);

    float max_line_width = text_width(title, MODAL_FONT, (int)MODAL_TITLE_FONT_SIZE);
    for (const std::string& line : lines) {
        float line_width = text_width(line, MODAL_FONT, (int)MODAL_MESSAGE_FONT_SIZE);
        if (line_width > max_line_width) {
            max_line_width = line_width;
        }
    }

    float modal_width = max_line_width + MODAL_PADDING * 2;
    if (modal_width < MODAL_MIN_WIDTH) modal_width = MODAL_MIN_WIDTH;

    float message_block_height = (int)lines.size() * MODAL_MESSAGE_FONT_SIZE + ((int)lines.size() - 1) * 6.0f;

    float buttons_total_height =
        (int)(buttons.size()) * MODAL_BUTTON_HEIGHT +
        ((int)(buttons.size()) - 1) * MODAL_BUTTON_GAP;

    float panel_height =
        MODAL_PADDING +
        MODAL_TITLE_FONT_SIZE +
        MODAL_PADDING +
        message_block_height +
        MODAL_PADDING +
        buttons_total_height +
        MODAL_PADDING;

    float panel_x = (window_width / 2.0f) - (modal_width / 2.0f);
    float panel_y = (window_height / 2.0f) - (panel_height / 2.0f);

    fill_rectangle(MODAL_COLOR_PANEL, panel_x, panel_y, modal_width, panel_height);
    draw_rectangle(MODAL_COLOR_PANEL_OUTLINE, panel_x, panel_y, modal_width, panel_height);

    float title_x = panel_x + (modal_width / 2.0f) - (text_width(title, MODAL_FONT, (int)MODAL_TITLE_FONT_SIZE) / 2.0f);
    float title_y = panel_y + MODAL_PADDING;
    draw_text(title, MODAL_COLOR_TITLE, MODAL_FONT, (int)MODAL_TITLE_FONT_SIZE, title_x, title_y);

    float current_y = title_y + MODAL_TITLE_FONT_SIZE + MODAL_PADDING;
    for (const std::string& line : lines) {
        float line_x = panel_x + (modal_width / 2.0f) - (text_width(line, MODAL_FONT, (int)MODAL_MESSAGE_FONT_SIZE) / 2.0f);
        draw_text(line, MODAL_COLOR_MESSAGE, MODAL_FONT, (int)MODAL_MESSAGE_FONT_SIZE, line_x, current_y);
        current_y += MODAL_MESSAGE_FONT_SIZE + 6.0f;
    }

    float button_width = (modal_width - MODAL_PADDING * 2 - MODAL_BUTTON_GAP * ((int)(buttons.size()) - 1)) / (int)(buttons.size());
    float buttons_y = current_y - 6.0f + MODAL_PADDING;
    float button_x = panel_x + MODAL_PADDING;

    for (int i = 0; i < (int)(buttons.size()); i++) {
        rectangle btn_rect = rectangle_from(button_x, buttons_y, button_width, MODAL_BUTTON_HEIGHT);
        bool clicked = unique_button(buttons[i].label, btn_rect);
        if (clicked) {
            play_sound_effect("ui_click");
            buttons[i].on_click();
        }
        button_x += button_width + MODAL_BUTTON_GAP;
    }
}