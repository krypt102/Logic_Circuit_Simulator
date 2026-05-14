#ifndef LOGIC_CIRCUIT_SIMULATOR_WIRE_HPP
#define LOGIC_CIRCUIT_SIMULATOR_WIRE_HPP

enum class WireConnectionType {
    GATE,
    INPUT_PIN,
    OUTPUT_PIN
};

struct Wire {
    int id;

    WireConnectionType from_type;
    int from_id;
    int from_pin_id;

    WireConnectionType to_type;
    int to_id;
    int to_pin_id;

    Wire(
        int id,
        WireConnectionType from_type,
        int from_id,
        int from_pin_id,

        WireConnectionType to_type,
        int to_id,
        int to_pin_id
    )
        : id(id),
          from_type(from_type),
          from_id(from_id),
          from_pin_id(from_pin_id),
          to_type(to_type),
          to_id(to_id),
          to_pin_id(to_pin_id)
    {}
};

#endif //LOGIC_CIRCUIT_SIMULATOR_WIRE_HPP
