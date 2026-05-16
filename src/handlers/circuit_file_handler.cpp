#include "circuit_file_handler.hpp"
#include <filesystem>
#include <fstream>

#include "../utils/terminal_utils.h"

bool CircuitFileHandler::save_circuit(const Circuit& circuit) {
    if (!std::filesystem::exists(SAVES_FOLDER)) {
        print_info("Saves folder not found, creating it");
        std::filesystem::create_directories(SAVES_FOLDER);
    }

    std::string file_path = build_file_path(circuit.circuit_name);
    print_info("Saving circuit to: " + file_path);

    std::ofstream file(file_path);
    if (!file.is_open()) {
        print_error("Failed to open file for saving: " + file_path);
        return false;
    }

    file << "name:" << circuit.circuit_name << "\n";
    file << "description:" << circuit.circuit_description << "\n";
    file << "next_id:" << circuit.next_id << "\n";

    // --- Gates ---
    // Format: gate:<id>,<type>,<x_position>,<y_position>
    for (const Gate& gate : circuit.circuit_gates) {
        file << "gate:"
             << gate.id << ","
             << gate_type_to_save_string(gate.gate_type) << ","
             << gate.x_position << ","
             << gate.y_position << "\n";
    }

    // --- Input pins ---
    // Format: input_pin:<id>,<x_position>,<y_position>,<value>
    for (const InputPin& pin : circuit.circuit_input_pins) {
        file << "input_pin:"
             << pin.id << ","
             << pin.x_position << ","
             << pin.y_position << ","
             << (pin.value ? "1" : "0") << "\n";
    }

    // --- Output pins ---
    // Format: output_pin:<id>,<x_position>,<y_position>
    for (const OutputPin& pin : circuit.circuit_output_pins) {
        file << "output_pin:"
             << pin.id << ","
             << pin.x_position << ","
             << pin.y_position << "\n";
    }

    // --- Wires ---
    // Format: wire:<id>,<from_type>,<from_id>,<from_pin_id>,<to_type>,<to_id>,<to_pin_id>
    for (const Wire& wire : circuit.circuit_wires) {
        file << "wire:"
             << wire.id << ","
             << wire_connection_type_to_string(wire.from_type) << ","
             << wire.from_id << ","
             << wire.from_pin_id << ","
             << wire_connection_type_to_string(wire.to_type) << ","
             << wire.to_id << ","
             << wire.to_pin_id << "\n";
    }

    print_info("Circuit saved successfully");
    return true;
}

std::optional<Circuit> CircuitFileHandler::load_circuit(const std::string& filename) {
    std::string file_path = build_file_path(filename);
    print_info("Loading circuit from: " + file_path);

    if (!std::filesystem::exists(file_path)) {
        print_error("Circuit file not found: " + file_path);
        return std::nullopt;
    }

    std::ifstream file(file_path);
    if (!file.is_open()) {
        print_error("Failed to open file for reading: " + file_path);
        return std::nullopt;
    }

    // 1. Load the name, description, and next_id

    std::string loaded_name;
    std::string loaded_description;
    int loaded_next_id = 0;

    bool found_name = false;
    bool found_description = false;
    bool found_next_id = false;

    // Lines with object details are stored and processed afterward
    // ... so if the main parts (header/desc/next_id) fail, we
    // ... don't bother with the objects (the file is gone)

    std::vector<std::string> object_lines;
    std::string line;

    int line_number = 0;
    while (std::getline(file, line)) {
        line_number++;

        // Skip the empty lines
        if (line.empty()) {
            continue;
        };

        // Remove annoying characters
        if (line.back() == '\r') {
            line.pop_back();
        }

        // Split on the first ':' to get key and value.
        size_t colon_pos = line.find(':');
        if (colon_pos == std::string::npos) {
            print_warning("Line " + std::to_string(line_number) + " missing ':', skipping");
            continue;
        }

        std::string key = line.substr(0, colon_pos);
        std::string value = line.substr(colon_pos + 1);

        if (key == "name") {
            loaded_name = value;
            found_name = true;
        } else if (key == "description") {
            loaded_description = value;
            found_description = true;
        } else if (key == "next_id") {
            // next_id must be positive and not empty
            bool is_valid_next_id = !value.empty();
            for (char current_char : value) {
                if (!std::isdigit(current_char)) {
                    is_valid_next_id = false;
                    break;
                }
            }
            if (!is_valid_next_id) {
                print_error("Line " + std::to_string(line_number) + " contains invalid next_id value: " + value);
                return std::nullopt;
            }
            loaded_next_id = to_integer(value);
            found_next_id = true;
        } else {
            // gate, input_pin, output_pin, wire, etc.
            object_lines.push_back(line);
        }
    }

    if (!found_name || !found_description || !found_next_id) {
        print_error("Circuit file is missing required header fields (name, description, next_id)");
        return std::nullopt;
    }

    Circuit circuit(loaded_name, loaded_description);
    circuit.next_id = loaded_next_id;

    // 2. If all the header data is good, work on the objects.
    for (const std::string& obj_line : object_lines) {
        size_t colon_pos = obj_line.find(':');
        std::string key = obj_line.substr(0, colon_pos);
        std::string value = obj_line.substr(colon_pos + 1);
        std::vector<std::string> parts = split_line(value);

        if (key == "gate") {
            // gate:<id>,<type>,<x_position>,<y_position>
            if (parts.size() != 4) {
                print_warning("Skipping broken gate line: " + obj_line);
                continue;
            }

            GateType gate_type;
            if (!string_to_gate_type(parts[1], gate_type)) {
                print_warning("Skipping gate with unknown type: " + parts[1]);
                continue;
            }

            int id = to_integer(parts[0]);

            // SplashKit with no "to_float" :(
            float x_position = std::stof(parts[2]);
            float y_position = std::stof(parts[3]);

            // We dont want to call add_gate because that increments next_id
            // ... we want it to retain its previous one.

            circuit.circuit_gates.emplace_back(id, gate_type, x_position, y_position);

        } else if (key == "input_pin") {
            // input_pin:<id>,<x_position>,<y_position>,<value>
            if (parts.size() != 4) {
                print_warning("Skipping broken input_pin line: " + obj_line);
                continue;
            }

            int id = to_integer(parts[0]);
            float x_position = std::stof(parts[1]);
            float y_position = std::stof(parts[2]);
            bool value_flag = (parts[3] == "1");

            InputPin pin(id, x_position, y_position);
            pin.value = value_flag;
            circuit.circuit_input_pins.push_back(pin);
        } else if (key == "output_pin") {
            // output_pin:<id>,<x_position>,<y_position>
            if (parts.size() != 3) {
                print_warning("Skipping broken output_pin line: " + obj_line);
                continue;
            }

            int id = to_integer(parts[0]);
            float x_position = std::stof(parts[1]);
            float y_position = std::stof(parts[2]);

            circuit.circuit_output_pins.emplace_back(id, x_position, y_position);

        } else if (key == "wire") {
            // wire:<id>,<from_type>,<from_id>,<from_pin_id>,<to_type>,<to_id>,<to_pin_id>
            if (parts.size() != 7) {
                print_warning("Skipping broken wire line: " + obj_line);
                continue;
            }

            WireConnectionType from_type, to_type;
            if (!string_to_wire_connection_type(parts[1], from_type)) {
                print_warning("Skipping wire with unknown from_type: " + parts[1]);
                continue;
            }
            if (!string_to_wire_connection_type(parts[4], to_type)) {
                print_warning("Skipping wire with unknown to_type: " + parts[4]);
                continue;
            }

            int id = to_integer(parts[0]);
            int from_id = to_integer(parts[2]);
            int from_pin_id = to_integer(parts[3]);
            int to_id = to_integer(parts[5]);
            int to_pin_id = to_integer(parts[6]);

            circuit.circuit_wires.emplace_back(id, from_type, from_id, from_pin_id, to_type, to_id, to_pin_id);
        } else {
            print_warning("Skipping unrecognised line: " + key);
        }
    }

    print_info("Circuit loaded successfully: " + circuit.circuit_name);
    return circuit;
}

bool CircuitFileHandler::save_exists(const std::string& filename) {
    return std::filesystem::exists(build_file_path(filename));
}

std::string CircuitFileHandler::build_file_path(const std::string& filename) {
    return SAVES_FOLDER + "/" + filename + CIRCUIT_FILE_EXTENSION;
}

std::string CircuitFileHandler::wire_connection_type_to_string(WireConnectionType type) {
    switch (type) {
        case WireConnectionType::GATE: return "GATE";
        case WireConnectionType::INPUT_PIN: return "INPUT_PIN";
        case WireConnectionType::OUTPUT_PIN: return "OUTPUT_PIN";
    }
    return "UNKNOWN";
}

bool CircuitFileHandler::string_to_wire_connection_type(const std::string& str, WireConnectionType& out_type) {
    if (str == "GATE") {
        out_type = WireConnectionType::GATE;
        return true;
    }
    if (str == "INPUT_PIN") {
        out_type = WireConnectionType::INPUT_PIN;
        return true;
    }
    if (str == "OUTPUT_PIN") {
        out_type = WireConnectionType::OUTPUT_PIN;
        return true;
    }
    return false;
}

std::string CircuitFileHandler::gate_type_to_save_string(GateType type) {
    return gate_type_to_string(type);
}

bool CircuitFileHandler::string_to_gate_type(const std::string& str, GateType& out_type) {
    if (str == "AND") {
        out_type = GateType::AND;
        return true;
    }
    if (str == "OR") {
        out_type = GateType::OR;
        return true;
    }
    if (str == "NOT") {
        out_type = GateType::NOT;
        return true;
    }
    if (str == "NAND") {
        out_type = GateType::NAND;
        return true;
    }
    if (str == "NOR") {
        out_type = GateType::NOR;
        return true;
    }
    if (str == "XOR") {
        out_type = GateType::XOR;
        return true;
    }
    return false;
}

std::vector<std::string> CircuitFileHandler::split_line(const std::string& line) {
    std::vector<std::string> split_parts;
    std::istringstream stream(line);
    std::string current_part;

    while (std::getline(stream, current_part, ',')) {
        split_parts.push_back(current_part);
    }
    return split_parts;
}