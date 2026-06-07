#ifndef LOGIC_CIRCUIT_SIMULATOR_SOUND_HANDLER_HPP
#define LOGIC_CIRCUIT_SIMULATOR_SOUND_HANDLER_HPP

#include <filesystem>
#include <format>
#include <vector>
#include "splashkit.h"
#include "../utils/terminal_utils.h"

class SoundHandler {
public:
    int init_sfx() {
        std::filesystem::path sound_folder = "Resources/sounds";
        if (!std::filesystem::exists(sound_folder) || !std::filesystem::is_directory(sound_folder)) {
            print_error("Sound folder not found: " + sound_folder.string());
            return 1;
        }

        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(sound_folder)) {
            std::string effect_name = entry.path().stem().string();
            std::string effect_path = entry.path().string();
            print_info(std::format(R"(Loading sound effect "{}" as "{}")", effect_path, effect_name));
            load_sound_effect(effect_name, effect_path);

            if (!has_sound_effect(effect_name)) {
                print_error(std::format(R"(Failed to load sound effect "{}")", effect_path));
                return 1;
            }
        }

        print_info("All sound effects loaded successfully");
        return 0;
    }

    int init_music_objects() {
        std::filesystem::path music_folder = "Resources/music";
        if (!std::filesystem::exists(music_folder) || !std::filesystem::is_directory(music_folder)) {
            print_error("Music folder not found: " + music_folder.string());
            return 1;
        }

        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(music_folder)) {
            std::string music_name = entry.path().stem().string();
            std::string music_path = entry.path().string();

            print_info(std::format(R"(Loading music "{}" as "{}")", music_path, music_name));
            load_music(music_name, music_path);

            music loaded = music_named(music_name);
            if (!loaded) {
                print_error(std::format(R"(Failed to load music "{}")", music_path));
                return 1;
            }
            loaded_music_tracks.push_back(loaded);
        }

        print_info("All music loaded successfully");
        return 0;
    }

    void free_music_objects() {
        for (music track : loaded_music_tracks) {
            free_music(track);
        }
    }

    // This is a bit of a cheat because SplashKit
    // ... doesn't technically have a set_sfx_volume() function
    void play_sfx(const std::string& name) const {
        play_sound_effect(name, 1, (float)(sfx_volume));
    }

    void set_sfx_volume(double volume) {
        sfx_volume = volume;
    }

    void set_bg_volume(double volume) const {
        set_music_volume((float)(volume));
    }

private:
    std::vector<music> loaded_music_tracks;
    double sfx_volume = 0.0;
};

#endif // LOGIC_CIRCUIT_SIMULATOR_SOUND_HANDLER_HPP
