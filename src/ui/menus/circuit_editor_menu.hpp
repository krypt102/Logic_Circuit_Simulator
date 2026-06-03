#ifndef LOGIC_CIRCUIT_SIMULATOR_CIRCUIT_EDITOR_MENU_HPP
#define LOGIC_CIRCUIT_SIMULATOR_CIRCUIT_EDITOR_MENU_HPP

#include "../../classes/Circuit.hpp"
#include "../../classes/Menu.hpp"
#include "../../handlers/settings_handler.hpp"
#include "splashkit.h"

enum class DragTarget {
    NONE,
    GATE,
    INPUT_PIN,
    OUTPUT_PIN
};

enum class PendingPlacement {
    NONE,
    GATE_AND,
    GATE_OR,
    GATE_NOT,
    GATE_NAND,
    GATE_NOR,
    GATE_XOR,
    GATE_XNOR,
    INPUT_PIN,
    OUTPUT_PIN
};

struct WireEndpoint {
    WireConnectionType object_type;
    int object_id;
    int pin_id;
};

class CircuitEditorMenu : public Menu {
public:
    CircuitEditorMenu(Circuit circuit, int window_width, int window_height, SettingsHandler& settings_handler);

    void on_enter(WindowHandler& window_handler, MenuHandler& menu_handler) override;
    void handle_input() override;
    void draw() const override;

private:
    Circuit circuit;
    int window_width;
    int window_height;

    SettingsHandler& settings_handler;

    bool show_grid = true;
    bool snap_to_grid = true;

    WindowHandler* window_handler = nullptr;
    MenuHandler* menu_handler = nullptr;

    DragTarget drag_target = DragTarget::NONE;
    int dragged_id = -1;
    float drag_offset_x = 0.0f;
    float drag_offset_y = 0.0f;

    bool mouse_left_just_pressed = false;

    bool drawing_wire = false;
    WireEndpoint wire_start;

    mutable PendingPlacement pending_placement = PendingPlacement::NONE;
    mutable int save_feedback_timer = 0;

    bool panning = false;
    float pan_offset_x = 0.0f;
    float pan_offset_y = 0.0f;
    float pan_start_mouse_x = 0.0f;
    float pan_start_mouse_y = 0.0f;
    float pan_start_offset_x = 0.0f;
    float pan_start_offset_y = 0.0f;

    void handle_mouse();
    void handle_toolbar();
    void handle_pan();

    void place_pending(float canvas_x, float canvas_y);

    void start_drag(float mouse_x, float mouse_y);
    void update_drag(float mouse_x, float mouse_y);
    void end_drag();

    void try_remove_at(float mouse_x, float mouse_y);

    float canvas_width() const;
    float snap_to_grid_value(float value) const;
    point_2d to_world_position(float screen_x, float screen_y) const;

    int gate_at(float x, float y) const;
    int input_pin_at(float x, float y) const;
    int output_pin_at(float x, float y) const;

    bool gate_pin_at(float x, float y, WireEndpoint& out_endpoint) const;

    bool connectable_pin_at(float x, float y, WireEndpoint& out_endpoint) const;
    void start_wire(float mouse_x, float mouse_y);
    void finish_wire(float mouse_x, float mouse_y);
    void cancel_wire();

    bool is_output_side(const WireEndpoint& endpoint) const;
    bool is_valid_wire_connection(const WireEndpoint& from, const WireEndpoint& to) const;

    int wire_at(float x, float y) const;

    void draw_grid() const;
    void draw_toolbar() const;
    void draw_sidebar() const;
    void draw_gates() const;
    void draw_input_pins() const;
    void draw_output_pins() const;
    void draw_wires() const;
    void draw_wire_in_progress(float mouse_x, float mouse_y) const;

    void draw_gate(const Gate& gate) const;
    void draw_input_pin(const InputPin& pin) const;
    void draw_output_pin(const OutputPin& pin) const;
    void draw_wire(const Wire& wire) const;

    point_2d connection_point_for(WireConnectionType type, int object_id, int pin_id) const;
};

#endif // LOGIC_CIRCUIT_SIMULATOR_CIRCUIT_EDITOR_MENU_HPP