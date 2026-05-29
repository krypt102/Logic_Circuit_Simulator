#include "circuit_editor_menu.hpp"
#include "../../handlers/menu_handler.hpp"
#include "../../utils/terminal_utils.h"
#include "../components/BackButton.hpp"
#include "splashkit.h"

const int GRID_CELL_SIZE = 32;
const float TOOLBAR_HEIGHT = 60.0f;

const float GATE_WIDTH = 80.0f;
const float GATE_HEIGHT = 60.0f;
const float GATE_PIN_RADIUS = 6.0f;

const float STANDALONE_PIN_RADIUS = 14.0f;

const std::string EDITOR_FONT = "JetBrainsMono-Regular";

const color COLOR_GATE_BODY = rgba_color(51, 120, 204, 255);
const color COLOR_GATE_LABEL = COLOR_WHITE;
const color COLOR_GATE_OUTLINE = COLOR_BLACK;

const color COLOR_PIN_HIGH = rgba_color(46, 184, 97, 255);
const color COLOR_PIN_LOW = rgba_color(140, 140, 140, 255);
const color COLOR_PIN_OUTLINE = COLOR_BLACK;
const color COLOR_PIN_LABEL = COLOR_BLACK;

const color COLOR_WIRE = rgba_color(38, 38, 38, 255);

CircuitEditorMenu::CircuitEditorMenu(Circuit circuit, int window_width, int window_height)
    : circuit(std::move(circuit)),
      window_width(window_width),
      window_height(window_height)
{
    print_info("CircuitEditorMenu created for circuit: " + this->circuit.circuit_name);
}

void CircuitEditorMenu::on_enter(WindowHandler& win_handler, MenuHandler& main_handler) {
    window_handler = &win_handler;
    menu_handler = &main_handler;
    print_info("Entered circuit editor: " + circuit.circuit_name);
}

void CircuitEditorMenu::handle_input() {}

void CircuitEditorMenu::draw() const {
    draw_grid();
    draw_wires();
    draw_gates();
    draw_input_pins();
    draw_output_pins();
    draw_toolbar();
}

void CircuitEditorMenu::draw_grid() const {
    float grid_top = TOOLBAR_HEIGHT;
    color grid_color = rgba_color(210, 210, 210, 255);

    for (int x = 0; x < window_width; x += GRID_CELL_SIZE) {
        draw_line(grid_color, x, grid_top, x, window_height);
    }
    for (int y = (int)(grid_top); y < window_height; y += GRID_CELL_SIZE) {
        draw_line(grid_color, 0, y, window_width, y);
    }
}

void CircuitEditorMenu::draw_toolbar() const {
    fill_rectangle(COLOR_LIGHT_GRAY, 0, 0, window_width, TOOLBAR_HEIGHT);

    bool clicked_back = draw_back_button();
    if (clicked_back) {
        play_sound_effect("ui_click");
        menu_handler->pop();
    }

    int font_size = 20;
    float name_x = (window_width / 2.0f) - (text_width(circuit.circuit_name, EDITOR_FONT, font_size) / 2.0f);
    float name_y = (TOOLBAR_HEIGHT / 2.0f) - (font_size / 2.0f);
    draw_text(circuit.circuit_name, COLOR_BLACK, EDITOR_FONT, font_size, name_x, name_y);
}

void CircuitEditorMenu::draw_gates() const {
    for (const Gate& gate : circuit.circuit_gates) {
        draw_gate(gate);
    }
}

void CircuitEditorMenu::draw_gate(const Gate& gate) const {
    float gate_x_position = gate.x_position;
    float gate_y_position = gate.y_position;

    fill_rectangle(COLOR_GATE_BODY, gate_x_position, gate_y_position, GATE_WIDTH, GATE_HEIGHT);
    draw_rectangle(COLOR_GATE_OUTLINE, gate_x_position, gate_y_position, GATE_WIDTH, GATE_HEIGHT);

    std::string label = gate_type_to_string(gate.gate_type);
    int font_size = 16;
    float label_x = gate_x_position + (GATE_WIDTH  / 2.0f) - (text_width(label, EDITOR_FONT, font_size) / 2.0f);
    float label_y = gate_y_position + (GATE_HEIGHT / 2.0f) - (font_size / 2.0f);
    draw_text(label, COLOR_GATE_LABEL, EDITOR_FONT, font_size, label_x, label_y);

    int input_count = (int)(gate.input_pins.size());
    float slot_height = GATE_HEIGHT / (input_count + 1);
    for (int i = 0; i < input_count; i++) {
        float pin_x = gate_x_position;
        float pin_y = gate_y_position + slot_height * (i + 1);
        fill_circle(COLOR_PIN_LOW, pin_x, pin_y, GATE_PIN_RADIUS);
        draw_circle(COLOR_GATE_OUTLINE, pin_x, pin_y, GATE_PIN_RADIUS);
    }

    float out_x = gate_x_position + GATE_WIDTH;
    float out_y = gate_y_position + (GATE_HEIGHT / 2.0f);
    fill_circle(COLOR_PIN_LOW, out_x, out_y, GATE_PIN_RADIUS);
    draw_circle(COLOR_GATE_OUTLINE, out_x, out_y, GATE_PIN_RADIUS);
}

void CircuitEditorMenu::draw_input_pins() const {
    for (const InputPin& pin : circuit.circuit_input_pins) {
        draw_input_pin(pin);
    }
}

void CircuitEditorMenu::draw_input_pin(const InputPin& pin) const {
    float pin_x_pos = pin.x_position;
    float pin_y_pos = pin.y_position;

    color body_color = pin.value ? COLOR_PIN_HIGH : COLOR_PIN_LOW;
    fill_circle(body_color, pin_x_pos, pin_y_pos, STANDALONE_PIN_RADIUS);
    draw_circle(COLOR_PIN_OUTLINE, pin_x_pos, pin_y_pos, STANDALONE_PIN_RADIUS);

    std::string label = "IN";
    int font_size = 12;
    float label_x = pin_x_pos - (text_width(label, EDITOR_FONT, font_size) / 2.0f);
    float label_y = pin_y_pos + STANDALONE_PIN_RADIUS + 2.0f;
    draw_text(label, COLOR_PIN_LABEL, EDITOR_FONT, font_size, label_x, label_y);
}

void CircuitEditorMenu::draw_output_pins() const {
    for (const OutputPin& pin : circuit.circuit_output_pins) {
        draw_output_pin(pin);
    }
}

void CircuitEditorMenu::draw_output_pin(const OutputPin& pin) const {
    float pin_x_pos = pin.x_position;
    float pin_y_pos = pin.y_position;

    color body_color = pin.value ? COLOR_PIN_HIGH : COLOR_PIN_LOW;
    fill_circle(body_color, pin_x_pos, pin_y_pos, STANDALONE_PIN_RADIUS);
    draw_circle(COLOR_PIN_OUTLINE, pin_x_pos, pin_y_pos, STANDALONE_PIN_RADIUS);

    std::string label = "OUT";
    int font_size = 12;
    float label_x = pin_x_pos - (text_width(label, EDITOR_FONT, font_size) / 2.0f);
    float label_y = pin_y_pos + STANDALONE_PIN_RADIUS + 2.0f;
    draw_text(label, COLOR_PIN_LABEL, EDITOR_FONT, font_size, label_x, label_y);
}

void CircuitEditorMenu::draw_wires() const {
    for (const Wire& wire : circuit.circuit_wires) {
        draw_wire(wire);
    }
}

void CircuitEditorMenu::draw_wire(const Wire& wire) const {
    point_2d from_point = get_connection_point_for(wire.from_type, wire.from_id, wire.from_pin_id);
    point_2d to_point = get_connection_point_for(wire.to_type, wire.to_id, wire.to_pin_id);
    draw_line(COLOR_WIRE, from_point.x, from_point.y, to_point.x, to_point.y);
}

point_2d CircuitEditorMenu::get_connection_point_for(
    WireConnectionType type,
    int object_id,
    int pin_id
) const {
    if (type == WireConnectionType::INPUT_PIN) {
        for (const InputPin& pin : circuit.circuit_input_pins) {
            if (pin.id == object_id) {
                return { pin.x_position, pin.y_position };
            }
        }
    }

    if (type == WireConnectionType::OUTPUT_PIN) {
        for (const OutputPin& pin : circuit.circuit_output_pins) {
            if (pin.id == object_id) {
                return { pin.x_position, pin.y_position };
            }
        }
    }

    if (type == WireConnectionType::GATE) {
        for (const Gate& gate : circuit.circuit_gates) {
            if (gate.id != object_id) {
                continue;
            };

            for (const Pin& p : gate.output_pins) {
                if (p.pin_id == pin_id) {
                    return {
                        gate.x_position + GATE_WIDTH,
                        gate.y_position + (GATE_HEIGHT / 2.0f)
                    };
                }
            }

            int input_count = (int)(gate.input_pins.size());
            float slot_height = GATE_HEIGHT / (input_count + 1);
            for (int i = 0; i < input_count; i++) {
                if (gate.input_pins[i].pin_id == pin_id) {
                    return {
                        gate.x_position,
                        gate.y_position + slot_height * (i + 1)
                    };
                }
            }
        }
    }

    print_warning("get_connection_point_for() could not calculate connection point for wire");
    return { 0.0f, 0.0f };
}