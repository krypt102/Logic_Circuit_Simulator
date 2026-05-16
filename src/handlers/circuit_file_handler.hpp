#ifndef LOGIC_CIRCUIT_SIMULATOR_CIRCUIT_FILE_HANDLER_HPP
#define LOGIC_CIRCUIT_SIMULATOR_CIRCUIT_FILE_HANDLER_HPP

#include <string>
#include <optional>
#include "../classes/Circuit.hpp"

const std::string SAVES_FOLDER = "saves";
const std::string CIRCUIT_FILE_EXTENSION = ".circuit";

class CircuitFileHandler {
public:
    bool save_circuit(const Circuit& circuit);
    bool save_exists(const std::string& filename);

    std::optional<Circuit> load_circuit(const std::string& filename);

private:
    std::string build_file_path(const std::string& filename);

    std::string wire_connection_type_to_string(WireConnectionType type);
    bool string_to_wire_connection_type(const std::string& str, WireConnectionType& out_type);

    std::string gate_type_to_save_string(GateType type);
    bool string_to_gate_type(const std::string& str, GateType& out_type);

    std::vector<std::string> split_csv(const std::string& line);
};

#endif //LOGIC_CIRCUIT_SIMULATOR_CIRCUIT_FILE_HANDLER_HPP
