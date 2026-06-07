#ifndef LOGIC_CIRCUIT_SIMULATOR_SETTINGS_HANDLER_HPP
#define LOGIC_CIRCUIT_SIMULATOR_SETTINGS_HANDLER_HPP

#include <filesystem>
#include <format>
#include <fstream>
#include <map>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <stdexcept>
#include <type_traits>

#include "splashkit.h"
#include "../utils/terminal_utils.h"

const std::string SETTINGS_FILE_PATH = "game_settings.txt";

const int DEFAULT_WIDTH = 1280;
const int DEFAULT_HEIGHT = 800;
const double DEFAULT_BG_VOL = 0.5;
const double DEFAULT_SFX_VOL = 0.75;
const bool DEFAULT_SHOW_GRID = true;
const bool DEFAULT_SNAP_GRID = true;

const std::map<std::string, std::string> DEFAULT_SETTINGS = {
    {"screenWidth", std::to_string(DEFAULT_WIDTH)},
    {"screenHeight", std::to_string(DEFAULT_HEIGHT)},
    {"bgVolume", std::to_string(DEFAULT_BG_VOL)},
    {"sfxVolume", std::to_string(DEFAULT_SFX_VOL)},
    {"showGrid", DEFAULT_SHOW_GRID ? "true" : "false"},
    {"snapToGrid", DEFAULT_SNAP_GRID ? "true" : "false"},
};

enum SettingType {
    SETTING_INT,
    SETTING_DOUBLE,
    SETTING_BOOL,
    SETTING_STRING
};

const std::map<std::string, SettingType> VALID_SETTING_TYPES = {
    {"screenWidth", SETTING_INT},
    {"screenHeight", SETTING_INT},
    {"bgVolume", SETTING_DOUBLE},
    {"sfxVolume", SETTING_DOUBLE},
    {"showGrid", SETTING_BOOL},
    {"snapToGrid", SETTING_BOOL},
};

class InvalidSettingKey {};
class InvalidSettingType {};

class SettingsHandler {
public:
    SettingsHandler() {
        print_info("SettingsHandler constructed");
    }

    void load_settings() {
        print_info("Loading settings");
        if (!std::filesystem::exists(SETTINGS_FILE_PATH)) {
            print_warning("No settings file found, creating defaults");
            save_settings();
        } else {
            parse_settings(read_settings_file());
        }
        print_info("Settings ready");
    }

    void save_settings() {
        print_info("Saving settings");
        std::ofstream file(SETTINGS_FILE_PATH);
        file << serialise_settings();
    }

    template<typename T>
    void set_setting(const std::string& key, T value) {
        if (!VALID_SETTING_TYPES.contains(key)) {
            print_error(std::format("Unknown setting key: {}", key));
            throw InvalidSettingKey();
        }

        std::string str_value;
        if constexpr (std::is_same_v<T, int>) {
            str_value = std::to_string(value);
        } else if constexpr (std::is_same_v<T, double>) {
            str_value = std::to_string(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            str_value = value ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            str_value = value;
        }

        if (!is_valid_setting(key, str_value)) {
            print_error(std::format("Invalid value for setting: {}", key));
            throw InvalidSettingKey();
        }

        settings[key] = str_value;
    }

    template<typename T>
    T get_setting(const std::string& key) {
        if (!(std::is_same_v<T, int> || std::is_same_v<T, double> || std::is_same_v<T, bool> || std::is_same_v<T, std::string>)) {
            print_error("get_setting only supports int, double, bool, or string");
            throw InvalidSettingType();
        }

        if (!VALID_SETTING_TYPES.contains(key)) {
            print_error(std::format("Unknown setting key: {}", key));
            throw InvalidSettingKey();
        }

        const std::string& val = settings.at(key);

        if constexpr (std::is_same_v<T, int>) {
            return to_integer(val);
        }
        if constexpr (std::is_same_v<T, double>) {
            return to_double(val);
        }
        if constexpr (std::is_same_v<T, bool>) {
            return val == "true";
        }
        if constexpr (std::is_same_v<T, std::string>) {
            return val;
        }
    }

private:
    std::map<std::string, std::string> settings = DEFAULT_SETTINGS;

    static bool is_bool_string(const std::string& val) {
        return val == "true" || val == "false";
    }

    bool is_valid_setting(const std::string& key, const std::string& val) const {
        SettingType expected = VALID_SETTING_TYPES.at(key);
        switch (expected) {
            case SETTING_INT: {
                return is_integer(val);
            }
            case SETTING_DOUBLE: {
                return is_double(val);
            }
            case SETTING_BOOL: {
                return is_bool_string(val);
            }
            case SETTING_STRING: {
                return true;
            }
        }
        return false;
    }

    std::string serialise_settings() const {
        std::string text;
        for (const auto& [key, val] : settings) {
            text += key + ":" + val + "\n";
        }
        return text;
    }

    std::string read_settings_file() const {
        std::string content;
        std::string line;
        std::ifstream file(SETTINGS_FILE_PATH);
        while (std::getline(file, line)) {
            content += line + "\n";
        }
        return content;
    }

    void revert_to_defaults() {
        settings = DEFAULT_SETTINGS;
        save_settings();
    }

    void parse_settings(const std::string& text) {
        std::map<std::string, std::string> parsed;
        std::istringstream stream(text);
        std::string line;

        while (std::getline(stream, line)) {
            if (line.empty()) {
                continue;
            }

            const size_t colon = line.find(':');
            if (colon == std::string::npos) {
                print_warning("Settings parse error - missing ':', reverting to defaults");
                revert_to_defaults();
                return;
            }

            const std::string key = line.substr(0, colon);
            const std::string val = line.substr(colon + 1);
            if (!VALID_SETTING_TYPES.contains(key) || !is_valid_setting(key, val)) {
                print_warning("Settings parse error - unknown key/value, reverting to defaults");
                revert_to_defaults();
                return;
            }

            parsed[key] = val;
        }

        for (const auto& key : VALID_SETTING_TYPES | std::views::keys) {
            if (!parsed.contains(key)) {
                print_warning(std::format(R"(Missing setting "{}", reverting to default)", key));
                revert_to_defaults();
                return;
            }
        }

        settings = parsed;
    }
};

#endif // LOGIC_CIRCUIT_SIMULATOR_SETTINGS_HANDLER_HPP
