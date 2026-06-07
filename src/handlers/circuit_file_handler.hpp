#ifndef LOGIC_CIRCUIT_SIMULATOR_CIRCUIT_FILE_HANDLER_HPP
#define LOGIC_CIRCUIT_SIMULATOR_CIRCUIT_FILE_HANDLER_HPP

#include <optional>
#include <string>
#include <vector>
#include "../classes/Circuit.hpp"

const std::string SAVES_FOLDER = "saves";
const std::string CIRCUIT_FILE_EXTENSION = ".circuit";

class CircuitFileHandler {
public:
    bool save_circuit(const Circuit& circuit);
    std::optional<Circuit> load_circuit(const std::string& filename);
    bool save_exists(const std::string& filename);
    bool delete_circuit(const std::string& filename);

private:
    std::string build_file_path(const std::string& filename) const;

    std::string wire_connection_type_to_string(WireConnectionType type) const;

    bool string_to_wire_connection_type(const std::string& str, WireConnectionType& out_type) const;

    bool string_to_gate_type(const std::string& str, GateType& out_type) const;

    std::vector<std::string> split_csv(const std::string& line) const;
};

#endif // LOGIC_CIRCUIT_SIMULATOR_CIRCUIT_FILE_HANDLER_HPP
