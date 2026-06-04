#ifndef LOGIC_CIRCUIT_SIMULATOR_GATE_HPP
#define LOGIC_CIRCUIT_SIMULATOR_GATE_HPP

#include <string>
#include <vector>
#include "splashkit.h"

#include "Pin.hpp"

enum class GateType {
    AND,
    OR,
    NOT,
    NAND,
    NOR,
    XOR,
    XNOR
};

std::string gate_type_to_string(GateType type);

color gate_color_for_type(GateType type);

color dragged_gate_color_for_type(GateType type);

int input_count_for_type(GateType type);

struct Gate {
    int id;
    float x_position;
    float y_position;

    GateType gate_type;
    std::vector<Pin> input_pins;
    std::vector<Pin> output_pins;

    Gate(int id, GateType type, float x, float y)
        : id(id),
          x_position(x),
          y_position(y),
          gate_type(type)
    {
        int input_count = input_count_for_type(type);
        for (int i = 0; i < input_count; i++) {
            input_pins.emplace_back(i, 0.0f, 0.0f, false);
        }
        output_pins.emplace_back(input_count, 0.0f, 0.0f, true);
    }
};

#endif //LOGIC_CIRCUIT_SIMULATOR_GATE_HPP
