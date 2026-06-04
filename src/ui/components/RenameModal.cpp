#include "RenameModal.hpp"
#include "../../handlers/menu_handler.hpp"
#include "UniqueButton.hpp"
#include "splashkit.h"

static const float RENAME_MODAL_WIDTH = 480.0f;
static const float RENAME_MODAL_PADDING = 32.0f;
static const float RENAME_MODAL_TITLE_SIZE = 22.0f;
static const float RENAME_MODAL_TEXTBOX_HEIGHT = 38.0f;
static const float RENAME_MODAL_BUTTON_HEIGHT = 42.0f;
static const float RENAME_MODAL_BUTTON_GAP = 12.0f;
static const float RENAME_MODAL_INNER_GAP = 20.0f;

static const std::string RM_FONT = "JetBrainsMono-Regular";

static const color RENAME_MODAL_COLOR_OVERLAY = rgba_color(0, 0, 0, 160);
static const color RENAME_MODAL_COLOR_PANEL = rgba_color(245, 245, 245, 255);
static const color RENAME_MODAL_COLOR_OUTLINE = rgba_color(180, 180, 180, 255);
static const color RENAME_MODAL_COLOR_TITLE = rgba_color(30, 30, 30, 255);

RenameModal::RenameModal(
    std::string title,
    std::string initial_value,
    std::function<void(const std::string&)> on_confirm
)
    : title(std::move(title)),
      current_value(std::move(initial_value)),
      on_confirm(std::move(on_confirm))
{}

void RenameModal::on_enter(WindowHandler& win_handler, MenuHandler& main_handler) {
    window_handler = &win_handler;
    menu_handler   = &main_handler;
    window_width   = win_handler.window_width;
    window_height  = win_handler.window_height;
}

void RenameModal::handle_input() {}

void RenameModal::draw() const {
    fill_rectangle(RENAME_MODAL_COLOR_OVERLAY, 0, 0, window_width, window_height);

    float button_row_width  = RENAME_MODAL_WIDTH - RENAME_MODAL_PADDING * 2;
    float single_btn_width  = (button_row_width - RENAME_MODAL_BUTTON_GAP) / 2.0f;

    float panel_height =
        RENAME_MODAL_PADDING +
        RENAME_MODAL_TITLE_SIZE +
        RENAME_MODAL_INNER_GAP +
        RENAME_MODAL_TEXTBOX_HEIGHT +
        RENAME_MODAL_INNER_GAP +
        RENAME_MODAL_BUTTON_HEIGHT +
        RENAME_MODAL_PADDING;

    float panel_x = (window_width  / 2.0f) - (RENAME_MODAL_WIDTH    / 2.0f);
    float panel_y = (window_height / 2.0f) - (panel_height / 2.0f);

    fill_rectangle(RENAME_MODAL_COLOR_PANEL, panel_x, panel_y, RENAME_MODAL_WIDTH, panel_height);
    draw_rectangle(RENAME_MODAL_COLOR_OUTLINE, panel_x, panel_y, RENAME_MODAL_WIDTH, panel_height);

    float title_x = panel_x + (RENAME_MODAL_WIDTH / 2.0f) - (text_width(title, RM_FONT, (int)RENAME_MODAL_TITLE_SIZE) / 2.0f);
    draw_text(title, RENAME_MODAL_COLOR_TITLE, RM_FONT, (int)RENAME_MODAL_TITLE_SIZE, title_x, panel_y + RENAME_MODAL_PADDING);

    float textbox_x = panel_x + RENAME_MODAL_PADDING;
    float textbox_y = panel_y + RENAME_MODAL_PADDING + RENAME_MODAL_TITLE_SIZE + RENAME_MODAL_INNER_GAP;
    current_value = text_box(current_value, rectangle_from(textbox_x, textbox_y, button_row_width, RENAME_MODAL_TEXTBOX_HEIGHT));

    float buttons_y = textbox_y + RENAME_MODAL_TEXTBOX_HEIGHT + RENAME_MODAL_INNER_GAP;
    float confirm_x = panel_x + RENAME_MODAL_PADDING;
    float cancel_x = confirm_x + single_btn_width + RENAME_MODAL_BUTTON_GAP;

    bool confirm_clicked = unique_button("Confirm", rectangle_from(confirm_x, buttons_y, single_btn_width, RENAME_MODAL_BUTTON_HEIGHT));
    bool cancel_clicked = unique_button("Cancel", rectangle_from(cancel_x,  buttons_y, single_btn_width, RENAME_MODAL_BUTTON_HEIGHT));

    if (confirm_clicked) {
        play_sound_effect("ui_click");
        on_confirm(current_value);
        play_sound_effect("click_success");
        menu_handler->pop();
    } else if (cancel_clicked) {
        play_sound_effect("ui_click");
        menu_handler->pop();
    }
}