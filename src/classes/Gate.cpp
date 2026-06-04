#include "Gate.hpp"

std::string gate_type_to_string(GateType type) {
    switch (type) {
        case GateType::AND:
            return "AND";
        case GateType::OR:
            return "OR";
        case GateType::NOT:
            return "NOT";
        case GateType::NAND:
            return "NAND";
        case GateType::NOR:
            return "NOR";
        case GateType::XOR:
            return "XOR";
        case GateType::XNOR:
            return "XNOR";
    }
    return "UNKNOWN";
}

color gate_color_for_type(GateType type) {
    switch (type) {
        case GateType::AND:
            return rgba_color(51, 120, 204, 255);
        case GateType::OR:
            return rgba_color(26, 158, 143, 255);
        case GateType::NOT:
            return rgba_color(224, 123, 32, 255);
        case GateType::NAND:
            return rgba_color(123, 76, 200, 255);
        case GateType::NOR:
            return rgba_color(200, 76, 110, 255);
        case GateType::XOR:
            return rgba_color(200, 160, 32, 255);
        case GateType::XNOR:
            return rgba_color(122, 158, 48, 255);
    }
    return rgba_color(51, 120, 204, 255);
}

color dragged_gate_color_for_type(GateType type) {
    switch (type) {
        case GateType::AND:
            return rgba_color(30, 80, 160, 255);
        case GateType::OR:
            return rgba_color(8, 110, 98, 255);
        case GateType::NOT:
            return rgba_color(160, 80, 10, 255);
        case GateType::NAND:
            return rgba_color(80, 40, 145, 255);
        case GateType::NOR:
            return rgba_color(145, 40, 70, 255);
        case GateType::XOR:
            return rgba_color(145, 108, 10, 255);
        case GateType::XNOR:
            return rgba_color(75, 108, 20, 255);
    }
    return rgba_color(30, 80, 160, 255);
}

int input_count_for_type(GateType type) {
    if (type == GateType::NOT) {
        return 1;
    };
    return 2;
}