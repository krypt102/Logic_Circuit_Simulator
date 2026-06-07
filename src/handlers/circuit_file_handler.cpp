#include "circuit_file_handler.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <sstream>
#include "../utils/terminal_utils.h"

bool CircuitFileHandler::save_circuit(const Circuit& circuit) {
    if (!std::filesystem::exists(SAVES_FOLDER)) {
        print_info("Saves folder not found, creating it");
        std::filesystem::create_directories(SAVES_FOLDER);
    }

    std::string file_path = build_file_path(circuit.circuit_name);
    print_info(std::format("Saving circuit to: {}", file_path));

    std::ofstream file(file_path);
    if (!file.is_open()) {
        print_error(std::format("Failed to open file for saving: {}", file_path));
        return false;
    }

    file << "name:" << circuit.circuit_name << "\n";
    file << "description:" << circuit.circuit_description << "\n";
    file << "next_id:" << circuit.next_id << "\n";

    // Format: gate:<id>,<type>,<x_position>,<y_position>
    for (const Gate& gate : circuit.circuit_gates) {
        file << "gate:"
             << gate.id << ","
             << gate_type_to_string(gate.gate_type) << ","
             << gate.x_position << ","
             << gate.y_position << "\n";
    }

    // Format: input_pin:<id>,<x_position>,<y_position>,<value>,<label>
    for (const InputPin& pin : circuit.circuit_input_pins) {
        file << "input_pin:"
             << pin.id << ","
             << pin.x_position << ","
             << pin.y_position << ","
             << (pin.value ? "1" : "0") << ","
             << pin.label << "\n";
    }

    // Format: output_pin:<id>,<x_position>,<y_position>,<label>
    for (const OutputPin& pin : circuit.circuit_output_pins) {
        file << "output_pin:"
             << pin.id << ","
             << pin.x_position << ","
             << pin.y_position << ","
             << pin.label << "\n";
    }

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
    print_info(std::format("Loading circuit from: {}", file_path));

    if (!std::filesystem::exists(file_path)) {
        print_error(std::format("Circuit file not found: {}", file_path));
        return std::nullopt;
    }

    std::ifstream file(file_path);
    if (!file.is_open()) {
        print_error(std::format("Failed to open file for reading: {}", file_path));
        return std::nullopt;
    }

    //  1. Load the name, description, and next_id
    std::string loaded_name;
    std::string loaded_description;
    int loaded_next_id = 0;

    bool found_name = false;
    bool found_description = false;
    bool found_next_id = false;

    std::vector<std::string> object_lines;
    std::string line;
    int line_number = 0;
    while (std::getline(file, line)) {
        line_number++;
        if (line.empty()) {
            continue;
        }

        if (line.back() == '\r') {
            line.pop_back();
        }

        size_t colon_pos = line.find(':');
        if (colon_pos == std::string::npos) {
            print_warning(std::format("Line {} is missing ':', skipping.", line_number));
            continue;
        }

        std::string key = line.substr(0, colon_pos);
        std::string val = line.substr(colon_pos + 1);

        if (key == "name") {
            loaded_name = val;
            found_name  = true;
        } else if (key == "description") {
            loaded_description = val;
            found_description  = true;
        } else if (key == "next_id") {
            bool valid = !val.empty();
            for (char current_char : val) {
                if (!std::isdigit(current_char)) {
                    valid = false;
                    break;
                }
            }
            if (!valid) {
                print_error(std::format("line {} has invalid next_id: {}", line_number, val));
                return std::nullopt;
            }
            loaded_next_id = to_integer(val);
            found_next_id  = true;
        } else {
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
        std::string val = obj_line.substr(colon_pos + 1);
        std::vector<std::string> parts = split_csv(val);

        if (key == "gate") {
            // gate:<id>,<type>,<x_position>,<y_position>
            if (parts.size() != 4) {
                print_warning(std::format("Skipping broken gate line: {}", obj_line));
                continue;
            }
            GateType gate_type;
            if (!string_to_gate_type(parts[1], gate_type)) {
                print_warning(std::format("Skipping gate with unknown type: {}", parts[1]));
                continue;
            }
            int id = to_integer(parts[0]);
            float x_position = std::stof(parts[2]);
            float y_position = std::stof(parts[3]);
            circuit.circuit_gates.emplace_back(id, gate_type, x_position, y_position);

        } else if (key == "input_pin") {
            // input_pin:<id>,<x_position>,<y_position>,<value>,<label>
            if (parts.size() < 4) {
                print_warning(std::format("Skipping broken input_pin line: {}", obj_line));
                continue;
            }
            int id = to_integer(parts[0]);
            float x_position = std::stof(parts[1]);
            float y_position = std::stof(parts[2]);
            bool value = (parts[3] == "1");

            InputPin pin(id, x_position, y_position);
            pin.value = value;
            if (parts.size() >= 5) {
                pin.label = parts[4];
            }
            circuit.circuit_input_pins.push_back(pin);
        } else if (key == "output_pin") {
            // output_pin:<id>,<x_position>,<y_position>,<label>
            if (parts.size() < 3) {
                print_warning(std::format("Skipping broken output_pin line: {}", obj_line));
                continue;
            }
            int id = to_integer(parts[0]);
            float x_position = std::stof(parts[1]);
            float y_position = std::stof(parts[2]);

            OutputPin pin(id, x_position, y_position);
            if (parts.size() >= 4) {
                pin.label = parts[3];
            }
            circuit.circuit_output_pins.push_back(pin);
        } else if (key == "wire") {
            // wire:<id>,<from_type>,<from_id>,<from_pin_id>,<to_type>,<to_id>,<to_pin_id>
            if (parts.size() != 7) {
                print_warning(std::format("Skipping broken wire line: {}", obj_line));
                continue;
            }
            WireConnectionType from_type, to_type;
            if (!string_to_wire_connection_type(parts[1], from_type)) {
                print_warning(std::format("Skipping wire with unknown from_type: {}", parts[1]));
                continue;
            }
            if (!string_to_wire_connection_type(parts[4], to_type)) {
                print_warning(std::format("Skipping wire with unknown to_type: {}", parts[4]));
                continue;
            }
            int id = to_integer(parts[0]);
            int from_id = to_integer(parts[2]);
            int from_pin_id = to_integer(parts[3]);
            int to_id = to_integer(parts[5]);
            int to_pin_id = to_integer(parts[6]);
            circuit.circuit_wires.emplace_back(id, from_type, from_id, from_pin_id, to_type, to_id, to_pin_id);
        } else {
            print_warning(std::format("Skipping unrecognized line: {}", key));
        }
    }

    print_info(std::format("Circuit loaded successfully: {}", circuit.circuit_name));
    return circuit;
}

bool CircuitFileHandler::save_exists(const std::string& filename) {
    return std::filesystem::exists(build_file_path(filename));
}

bool CircuitFileHandler::delete_circuit(const std::string& filename) {
    std::string file_path = build_file_path(filename);
    if (!std::filesystem::exists(file_path)) {
        print_error(std::format("Cannot delete circuit - file not found: {}", file_path));
        return false;
    }
    std::filesystem::remove(file_path);
    print_info(std::format("Deleted circuit file: {}", file_path));
    return true;
}

std::string CircuitFileHandler::build_file_path(const std::string& filename) const {
    return std::format("{}/{}{}", SAVES_FOLDER, filename, CIRCUIT_FILE_EXTENSION);
}

std::string CircuitFileHandler::wire_connection_type_to_string(WireConnectionType type) const {
    switch (type) {
        case WireConnectionType::GATE: {
            return "GATE";
        }
        case WireConnectionType::INPUT_PIN: {
            return "INPUT_PIN";
        }
        case WireConnectionType::OUTPUT_PIN: {
            return "OUTPUT_PIN";
        }
    }
    return "UNKNOWN";
}

bool CircuitFileHandler::string_to_wire_connection_type(const std::string& str, WireConnectionType& out_type) const {
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

bool CircuitFileHandler::string_to_gate_type(const std::string& str, GateType& out_type) const {
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
    if (str == "XNOR") {
        out_type = GateType::XNOR;
        return true;
    }
    return false;
}

std::vector<std::string> CircuitFileHandler::split_csv(const std::string& line) const {
    std::vector<std::string> parts;
    std::istringstream stream(line);
    std::string part;
    while (std::getline(stream, part, ',')) {
        parts.push_back(part);
    }
    return parts;
}