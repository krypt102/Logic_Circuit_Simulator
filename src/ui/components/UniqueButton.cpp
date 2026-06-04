#include "UniqueButton.hpp"

int button_counter = 0;

void reset_button_counter() {
    button_counter = 0;
}

bool unique_button(const std::string& label, const rectangle &rect) {
    std::string unique_label = label;
    for (int i = 0; i < button_counter; i++) {
        unique_label += "\u200B";
    }
    button_counter++;
    return button(unique_label, rect);
}