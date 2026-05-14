#ifndef LOGIC_CIRCUIT_SIMULATOR_SOUND_HANDLER_HPP
#define LOGIC_CIRCUIT_SIMULATOR_SOUND_HANDLER_HPP

#include <filesystem>
#include <format>
#include "splashkit.h"
#include "../utils/terminal_utils.h"

namespace filesystem = std::filesystem;

class SoundHandler {
private:
    vector<music> music_objects;

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

    int init_music_objects() {
        filesystem::path music_folder_path = "Resources\\music";
        if (!filesystem::exists(music_folder_path) || !filesystem::is_directory(music_folder_path)) {
            print_error("Failed to load music folder");
            return 1;
        }

        for (const filesystem::directory_entry& current_file : filesystem::directory_iterator(music_folder_path)) {
            print_info(std::format(
                R"(Loading music "{}" as "{}")",
                current_file.path().string(),
                current_file.path().stem().string()
            ));
            load_music(current_file.path().stem().string(), current_file.path().string());

            music loaded_music = music_named(current_file.path().stem().string());
            if (!loaded_music) {
                print_error(std::format("Failed to load music \"{}\"", current_file.path().string()));
                return 1;
            }
            music_objects.push_back(loaded_music);
        }

        print_info("Initialised music successfully");
        return 0;
    }

    void free_music_objects() const {
        for (const music &music_object : music_objects) {
            free_music(music_object);
        }
    }
};

#endif // LOGIC_CIRCUIT_SIMULATOR_SOUND_HANDLER_HPP