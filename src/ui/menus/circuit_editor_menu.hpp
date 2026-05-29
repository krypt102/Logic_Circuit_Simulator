#ifndef LOGIC_CIRCUIT_SIMULATOR_CIRCUIT_EDITOR_MENU_HPP
#define LOGIC_CIRCUIT_SIMULATOR_CIRCUIT_EDITOR_MENU_HPP

#include "../../classes/Circuit.hpp"
#include "../../classes/Menu.hpp"
#include "splashkit.h"

class CircuitEditorMenu : public Menu {
public:
    CircuitEditorMenu(Circuit circuit, int window_width, int window_height);

    void on_enter(WindowHandler& window_handler, MenuHandler& menu_handler) override;
    void handle_input() override;
    void draw() const override;

private:
    Circuit circuit;
    int window_width;
    int window_height;

    WindowHandler* window_handler = nullptr;
    MenuHandler* menu_handler   = nullptr;

    void draw_grid() const;
    void draw_toolbar() const;
    void draw_gates() const;
    void draw_input_pins() const;
    void draw_output_pins() const;
    void draw_wires()  const;

    void draw_gate(const Gate& gate) const;
    void draw_input_pin(const InputPin& pin) const;
    void draw_output_pin(const OutputPin& pin) const;
    void draw_wire(const Wire& wire) const;

    point_2d get_connection_point_for(WireConnectionType type, int object_id, int pin_id) const;
};

#endif //LOGIC_CIRCUIT_SIMULATOR_CIRCUIT_EDITOR_MENU_HPP
