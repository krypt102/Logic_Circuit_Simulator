#ifndef LOGIC_CIRCUIT_SIMULATOR_FONT_HANDLER_HPP
#define LOGIC_CIRCUIT_SIMULATOR_FONT_HANDLER_HPP

#include <filesystem>
#include "splashkit.h"
#include "../utils/terminal_utils.h"
#include <format>

namespace filesystem = std::filesystem;

class FontHandler {
public:
    int init_fonts() {
        // This preloads the required fonts for the game...
        // ... before any components load so SplashKit doesn't error.

        filesystem::path font_folder_path = "Resources\\fonts";
        if (!filesystem::exists(font_folder_path) || !filesystem::is_directory(font_folder_path)) {
            print_error("Failed to load font folder");
            return 1;
        }

        for (const filesystem::directory_entry& current_file : filesystem::directory_iterator(font_folder_path)) {
            print_info(std::format(
                R"(Loading font "{}" as "{}")",
                current_file.path().string(),
                current_file.path().stem().string()
            ));
            font loaded_font = load_font(current_file.path().stem().string(), current_file.path().string());
            if (!has_font(loaded_font)) {
                print_error(std::format("Failed to load font \"{}\"", current_file.path().string()));
                return 1;
            }
        }

        // load_font("JetBrainsMono_Reg", "JetBrainsMono-Regular.ttf");
        print_info("Initialised fonts successfully");
        // print_error("Deliberate error to halt");
        return 1;
    }
};

#endif //LOGIC_CIRCUIT_SIMULATOR_FONT_HANDLER_HPP
