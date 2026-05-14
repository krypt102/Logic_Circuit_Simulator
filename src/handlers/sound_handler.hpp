#ifndef LOGIC_CIRCUIT_SIMULATOR_SOUND_HANDLER_HPP
#define LOGIC_CIRCUIT_SIMULATOR_SOUND_HANDLER_HPP

#include "splashkit.h"
#include <filesystem>
#include "../utils/terminal_utils.h"
#include <format>

namespace filesystem = std::filesystem;
class SoundHandler {
public:
    int init_sfx() {
        filesystem::path sound_folder_path = "Resources\\sounds";
        if (!filesystem::exists(sound_folder_path) || !filesystem::is_directory(sound_folder_path)) {
            print_error("Failed to load sound folder");
            return 1;
        }

        for (const filesystem::directory_entry& current_file : filesystem::directory_iterator(sound_folder_path)) {
            print_info(std::format(
                R"(Loading sound effect "{}" as "{}")",
                current_file.path().string(),
                current_file.path().stem().string()
            ));
            load_sound_effect(current_file.path().stem().string(), current_file.path().string());
            if (!has_sound_effect(current_file.path().stem().string())) {
                print_error(std::format("Failed to load sound effect \"{}\"", current_file.path().string()));
                return 1;
            }
        }

        print_info("Initialised sound effects successfully");
        return 0;
    }
};

#endif //LOGIC_CIRCUIT_SIMULATOR_SOUND_HANDLER_HPP
