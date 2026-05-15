#include "BackButton.hpp"
#include "splashkit.h"

bool draw_back_button() {
    float back_button_x = 20.0f;
    float back_button_y = 20.0f;
    float back_button_width = 100.0f;
    float back_button_height = 52.0f;

    bool has_clicked_back = button("Back", rectangle_from(back_button_x, back_button_y, back_button_width, back_button_height));
    return has_clicked_back;
}