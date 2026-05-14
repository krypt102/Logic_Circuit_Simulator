#ifndef LOGIC_CIRCUIT_SIMULATOR_PIN_HPP
#define LOGIC_CIRCUIT_SIMULATOR_PIN_HPP

struct Pin {
    int pin_id;
    float x_position;
    float y_position;
    bool is_output_pin;
    int connected_wire_id;

    Pin(int id, float x, float y, bool is_output)
        : pin_id(id),
        x_position(x),
        y_position(y),
        is_output_pin(is_output),
        connected_wire_id(-1)
    {};
};

struct InputPin {
    int id;
    float x_position;
    float y_position;
    bool value;
    int connected_wire_id;

    InputPin(int id, float x, float y)
        : id(id),
          x_position(x),
          y_position(y),
          value(false),
          connected_wire_id(-1)
    {}
};

struct OutputPin {
    int id;
    float x_position;
    float y_position;
    bool value;
    int connected_wire_id;

    OutputPin(int id, float x, float y)
        : id(id),
          x_position(x),
          y_position(y),
          value(false),
          connected_wire_id(-1) {}
};

#endif //LOGIC_CIRCUIT_SIMULATOR_PIN_HPP
