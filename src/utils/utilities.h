#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <vector>

// Returns true if the given name is a valid circuit filename.
// Names cannot contain bad letters
bool is_valid_circuit_name(const std::string& name);

// Wraps text into multiple lines no larger than max_width
std::vector<std::string> wrap_text(
    const std::string& text,
    float max_width,
    const std::string& font = "JetBrainsMono-Regular",
    int font_size = 24
);

std::vector<std::string> split_lines(const std::string& text);

#endif
