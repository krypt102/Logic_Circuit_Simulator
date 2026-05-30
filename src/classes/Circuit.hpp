#ifndef LOGIC_CIRCUIT_SIMULATOR_CIRCUIT_HPP
#define LOGIC_CIRCUIT_SIMULATOR_CIRCUIT_HPP

#include <string>
#include <vector>
#include "Gate.hpp"
#include "Pin.hpp"
#include "Wire.hpp"

struct Circuit {
    std::string circuit_name;
    std::string circuit_description;
    std::vector<Gate> circuit_gates;
    std::vector<InputPin> circuit_input_pins;
    std::vector<OutputPin> circuit_output_pins;
    std::vector<Wire> circuit_wires;

    int next_id; // Next id to assign to the next created object (every one must have a unique ID)

    Circuit(const std::string& name, const std::string& description)
        : circuit_name(name),
          circuit_description(description),
          next_id(0)
    {}

    // ------Gates-----------------------------------------------------
    // -------------------------------------------------------------------------

    int add_gate(GateType type, float x, float y) {
        int gate_id = next_id++;
        circuit_gates.emplace_back(gate_id, type, x, y);
        return gate_id;
    }

    void remove_gate(int gate_id) {
        remove_wires_connected_to(gate_id, WireConnectionType::GATE);

        std::erase_if(circuit_gates, [gate_id](const Gate& gate) {
            return gate.id == gate_id;
        });
    }

    // Returns a pointer to the gate with the given id.
    Gate* find_gate_with_id(int gate_id) {
        for (Gate& gate : circuit_gates) {
            if (gate.id == gate_id) return &gate;
        }
        return nullptr;
    }

    // ------Input Pins-----------------------------------------------------
    // -------------------------------------------------------------------------

    int add_input_pin(float x, float y) {
        int pin_id = next_id++;
        circuit_input_pins.emplace_back(pin_id, x, y);
        return pin_id;
    }

    void remove_input_pin(int pin_id) {
        remove_wires_connected_to(pin_id, WireConnectionType::INPUT_PIN);
        std::erase_if(circuit_input_pins, [pin_id](const InputPin& pin) {
            return pin.id == pin_id;
        });
    }

    InputPin* find_input_pin_by_id(int pin_id) {
        for (InputPin& pin : circuit_input_pins) {
            if (pin.id == pin_id) {
                return &pin;
            };
        }
        return nullptr;
    }

    void toggle_input_pin(int pin_id) {
        InputPin* pin = find_input_pin_by_id(pin_id);
        if (pin) {
            pin->value = !pin->value;
        };
    }

    // ------Output Pins-----------------------------------------------------
    // -------------------------------------------------------------------------

    int add_output_pin(float x, float y) {
        int pin_id = next_id++;
        circuit_output_pins.emplace_back(pin_id, x, y);
        return pin_id;
    }

    void remove_output_pin(int pin_id) {
        remove_wires_connected_to(pin_id, WireConnectionType::OUTPUT_PIN);

        std::erase_if(circuit_output_pins, [pin_id](const OutputPin& pin) {
            return pin.id == pin_id;
        });
    }

    OutputPin* find_output_pin_by_id(int pin_id) {
        for (OutputPin& pin : circuit_output_pins) {
            if (pin.id == pin_id) {
                return &pin;
            };
        }
        return nullptr;
    }

    // ------Circuit Wires-----------------------------------------------------
    // -------------------------------------------------------------------------

    int add_wire(
        WireConnectionType from_type,
        int from_id,
        int from_pin_id,

        WireConnectionType to_type,
        int to_id,
        int to_pin_id
    ){
        int wire_id = next_id++;
        circuit_wires.emplace_back(wire_id, from_type, from_id, from_pin_id, to_type, to_id, to_pin_id);
        return wire_id;
    }

    void remove_wire(int wire_id) {
        std::erase_if(circuit_wires, [wire_id](const Wire& wire) {
            return wire.id == wire_id;
        });
    }

    Wire* find_wire_by_id(int wire_id) {
        for (Wire& wire : circuit_wires) {
            if (wire.id == wire_id) {
                return &wire;
            };
        }
        return nullptr;
    }

    // ------Circuit Simulation-----------------------------------------------------
    // -------------------------------------------------------------------------

    void simulate() {}


private:
    void remove_wires_connected_to(int object_id, WireConnectionType node_type) {
        std::erase_if(circuit_wires, [object_id, node_type](const Wire& wire) {
            bool from_matches = (wire.from_type == node_type && wire.from_id == object_id);
            bool to_matches = (wire.to_type == node_type && wire.to_id == object_id);
            return from_matches || to_matches;
        });
    }
};

#endif //LOGIC_CIRCUIT_SIMULATOR_CIRCUIT_HPP
