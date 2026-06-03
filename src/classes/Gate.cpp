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

int input_count_for_type(GateType type) {
    if (type == GateType::NOT) {
        return 1;
    };
    return 2;
}