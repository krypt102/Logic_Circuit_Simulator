#ifndef LOGIC_CIRCUIT_SIMULATOR_FONT_HANDLER_HPP
#define LOGIC_CIRCUIT_SIMULATOR_FONT_HANDLER_HPP

#include <filesystem>
#include <format>
#include "splashkit.h"
#include "../utils/terminal_utils.h"


class FontHandler {
public:
    int init_fonts() {
        std::filesystem::path font_folder = "Resources/fonts";
        if (!std::filesystem::exists(font_folder) || !std::filesystem::is_directory(font_folder)) {
            print_error("Font folder not found: " + font_folder.string());
            return 1;
        }

        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(font_folder)) {
            std::string font_name = entry.path().stem().string();
            std::string font_path = entry.path().string();
            print_info(std::format(R"(Loading font "{}" as "{}")", font_path, font_name));

            font loaded_font = load_font(font_name, font_path);
            if (!has_font(loaded_font)) {
                print_error(std::format(R"(Failed to load font "{}")", font_path));
                return 1;
            }
        }

        print_info("All fonts loaded successfully");
        return 0;
    }
};

#endif // LOGIC_CIRCUIT_SIMULATOR_FONT_HANDLER_HPP
