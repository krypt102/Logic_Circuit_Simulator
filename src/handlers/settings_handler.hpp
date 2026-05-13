#ifndef LOGIC_CIRCUIT_SIMULATOR_SETTINGS_HANDLER_HPP
#define LOGIC_CIRCUIT_SIMULATOR_SETTINGS_HANDLER_HPP

#include <filesystem>
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

using namespace std;

const string SETTINGS_FILE_PATH = "game_settings.txt";
const int DEFAULT_WIDTH = 1280;
const int DEFAULT_HEIGHT = 720;

const map<string, string> DEFAULT_SETTINGS = {
    {"screenWidth",  std::to_string(DEFAULT_WIDTH)},
    {"screenHeight", std::to_string(DEFAULT_HEIGHT)},
};

enum settings_type {
    INT,
    DOUBLE,
    BOOL,
    STRING
};

const map<string, settings_type> VALID_SETTING_TYPES = {
    {"screenWidth", INT},
    {"screenHeight", INT},
};

class invalid_setting_key {};
class invalid_type {};

class SettingsHandler {
private:
    map<string, string> settings = DEFAULT_SETTINGS;

    static bool is_bool_value(const string& val) {
        return val == "true" || val == "false";
    }

    bool is_valid_setting(const string& key, const string& val) {
        const settings_type expected_type = VALID_SETTING_TYPES.at(key);

        switch (expected_type) {
            case INT:
                return is_integer(val);
            case DOUBLE:
                return is_double(val);
            case BOOL:
                return is_bool_value(val);
            case STRING:
                return true;
        }
        return false;
    }

    string serialise_settings() {
        string text;
        for (const auto& [key, val] : settings) {
            text += key;
            text += ":";
            text += val;
            text += "\n";
        }
        return text;
    }

    string read_settings_file() {
        string file_text;
        string line;

        ifstream settings_file(SETTINGS_FILE_PATH);
        while (getline(settings_file, line)) {
            file_text += line;
            file_text += "\n";
        }
        return file_text;
    }

    void revert_to_defaults() {
        settings = DEFAULT_SETTINGS;
        save_settings();
    }

    void parse_settings(const string& text) {
        map<string, string> parsed;
        istringstream stream(text);
        string line;

        while (getline(stream, line)) {
            if (line.empty()) {
                continue;
            };

            const size_t delimiter_pos = line.find(':');
            if (delimiter_pos == string::npos) {
                print_warning("Setting parse error, reverting to default");
                revert_to_defaults();
                return;
            }

            const string key = line.substr(0, delimiter_pos);
            const string val = line.substr(delimiter_pos + 1);

            if (!VALID_SETTING_TYPES.contains(key) || !is_valid_setting(key, val)) {
                print_warning("Setting parse error, reverting to default");
                revert_to_defaults();
                return;
            }

            parsed[key] = val;
        }

        for (const auto& key : VALID_SETTING_TYPES | views::keys) {
            if (!parsed.contains(key)) {
                string warning = "Missing setting '";
                warning += key;
                warning += "', reverting to default";
                print_warning(warning);
                revert_to_defaults();
                return;
            }
        }

        settings = parsed;
    }

public:
    SettingsHandler() {
        print_info("Settings handler constructed");
    }

    void load_settings() {
        print_info("Checking settings file");

        if (!filesystem::exists(SETTINGS_FILE_PATH)) {
            print_info("No settings file found, assigning defaults");
            save_settings();
        } else {
            print_info("Loading settings file");
            parse_settings(read_settings_file());
        }
        print_info("Settings initialised");
    }

    void save_settings() {
        print_info("Saving settings file");
        ofstream settings_file(SETTINGS_FILE_PATH);
        settings_file << serialise_settings();
    }

    template<typename T>
    T get_setting(const string& key) {
        if (!(is_same_v<T, int> || is_same_v<T, double> || is_same_v<T, bool> || is_same_v<T, string>)) {
            print_error("get_setting only supports int, double, bool, or string");
            throw invalid_type();
        };

        if (!VALID_SETTING_TYPES.contains(key)) {
            print_error("Unknown setting key: " + key);
            throw invalid_setting_key();
        }

        const string &val = settings.at(key);

        if constexpr (is_same_v<T, int>) {
            return to_integer(val);
        }
        if constexpr (is_same_v<T, double>) {
            return to_double(val);
        }
        if constexpr (is_same_v<T, bool>) {
            return val == "true";
        }
        if constexpr (is_same_v<T, string>) {
            return val;
        };
    }
};

#endif // LOGIC_CIRCUIT_SIMULATOR_SETTINGS_HANDLER_HPP