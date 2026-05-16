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