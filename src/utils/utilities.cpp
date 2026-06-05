#include "splashkit.h"
#include "utilities.h"
#include <sstream>

void print_repeated(string text_to_print, int times_to_print, bool with_newline) {
    for (int i = 0; i < times_to_print; i++) {
        write(text_to_print);
    }
    if (with_newline) {
        write('\n');
    }
}

void print_line(int line_length, bool with_newline) {
    print_repeated("-", line_length, with_newline);
}

string read_string(string prompt) {
    write(prompt);
    return read_line();
}

int read_integer(string prompt) {
    write(prompt);
    string line = read_line();
    while (!is_integer(line)) {
        write_line("Please enter an integer.");
        write(prompt);
        line = read_line();
    }
    return to_integer(line);
}

int read_integer_range(string prompt, int range_min, int range_max) {
    int integer_input = read_integer(prompt);
    while (integer_input < range_min || integer_input > range_max) {
        write_line("Please enter an integer within the range " + to_string(range_min) + " to " + to_string(range_max));
        integer_input = read_integer(prompt);
    }
    return integer_input;
}

double read_double(string prompt) {
    write(prompt);
    string line = read_line();
    while (!is_double(line)) {
        write_line("Please enter an integer or decimal value");
        write(prompt);
        line = read_line();
    }
    return to_double(line);
}

double read_double_range(string prompt, double range_min, double range_max) {
    double double_input = read_double(prompt);
    while (double_input < range_min || double_input > range_max) {
        write_line("Please enter an integer or decimal within the range " + to_string(range_min) + " to " + to_string(range_max));
        double_input = read_integer(prompt);
    }
    return double_input;
}

string read_line_binary(string prompt) {
    string user_input;
    while (true) {
        write( prompt + " [Y/N]: ");
        user_input = to_lowercase(read_line());

        if (user_input == "y" || user_input == "n") {
            break;
        }
        write_line("Please enter a valid choice.");
    }

    return user_input;
}

bool is_valid_circuit_name(const string& name) {
    if (name.empty()) {
        return false;
    }
    const string illegal_chars = "/\\:*?\"<>|";
    for (char current_char : name) {
        if (illegal_chars.find(current_char) != string::npos) {
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