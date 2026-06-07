#include "splashkit.h"
#include "utilities.h"

bool is_valid_circuit_name(const std::string& name) {
    if (name.empty()) {
        return false;
    }
    const std::string illegal_chars = "/\\:*?\"<>|";
    for (char current_char : name) {
        if (illegal_chars.find(current_char) != std::string::npos) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> wrap_text(
    const std::string& text,
    float max_width,
    const std::string& font,
    int font_size
) {
    std::vector<std::string> lines;
    std::string current_line;

    for (char current_char : text) {
        std::string currently_testing = current_line + current_char;
        if (text_width(currently_testing, font, font_size) > max_width) {
            lines.push_back(current_line);
            current_line = current_char;
        } else {
            current_line = currently_testing;
        }
    }

    if (!current_line.empty()) {
        lines.push_back(current_line);
    }

    return lines;
}

std::vector<std::string> split_lines(const std::string& text) {
    std::vector<std::string> lines;
    std::string current;
    for (char current_char : text) {
        if (current_char == '\n') {
            lines.push_back(current);
            current.clear();
        } else {
            current += current_char;
        }
    }
    lines.push_back(current);
    return lines;
}
