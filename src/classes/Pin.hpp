#ifndef LOGIC_CIRCUIT_SIMULATOR_PIN_HPP
#define LOGIC_CIRCUIT_SIMULATOR_PIN_HPP

struct Pin {
    int pin_id;
    float x_position;
    float y_position;
    bool is_output_pin;
    bool value;

    Pin(int id, float x, float y, bool is_output)
        : pin_id(id),
        x_position(x),
        y_position(y),
        is_output_pin(is_output),
        value(false)
    {};
};

struct InputPin {
    int id;
    float x_position;
    float y_position;
    bool value;

    InputPin(int id, float x, float y)
        : id(id),
          x_position(x),
          y_position(y),
          value(false)
    {}
};

struct OutputPin {
    int id;
    float x_position;
    float y_position;
    bool value;

    OutputPin(int id, float x, float y)
        : id(id),
          x_position(x),
          y_position(y),
          value(false)
    {}
};

#endif //LOGIC_CIRCUIT_SIMULATOR_PIN_HPP
