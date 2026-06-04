#include "circuit_editor_menu.hpp"

#include <format>

#include "edit_circuit_details_menu.hpp"
#include "../../handlers/menu_handler.hpp"
#include "../../utils/terminal_utils.h"
#include "../../handlers/circuit_file_handler.hpp"
#include "../components/BackButton.hpp"
#include "../components/UniqueButton.hpp"
#include "../components/RenameModal.hpp"
#include "splashkit.h"

const int GRID_CELL_SIZE = 32;
const float TOOLBAR_HEIGHT = 80.0f;
const float SIDEBAR_WIDTH = 180.0f;
const float SIDEBAR_BUTTON_HEIGHT = 44.0f;
const float SIDEBAR_BUTTON_GAP = 8.0f;
const float SIDEBAR_PADDING = 12.0f;

const float GATE_WIDTH = 80.0f;
const float GATE_HEIGHT = 60.0f;
const float GATE_PIN_RADIUS = 6.0f;

const float STANDALONE_PIN_RADIUS = 14.0f;

const float GATE_PIN_HIT_RADIUS = 10.0f;
const float WIRE_HIT_TOLERANCE = 6.0f;

const std::string EDITOR_FONT = "JetBrainsMono-Regular";

const color COLOR_GATE_LABEL = COLOR_WHITE;
const color COLOR_GATE_OUTLINE = COLOR_BLACK;

const color COLOR_PIN_HIGH = rgba_color(46, 184, 97, 255);
const color COLOR_PIN_LOW = rgba_color(140, 140, 140, 255);
const color COLOR_PIN_DRAGGED = rgba_color(90, 90, 90, 255);
const color COLOR_PIN_OUTLINE = COLOR_BLACK;
const color COLOR_PIN_LABEL = COLOR_BLACK;

const color COLOR_WIRE = rgba_color(38, 38, 38, 255);
const color COLOR_WIRE_HIGH = rgba_color(46, 184, 97, 255);
const color COLOR_WIRE_IN_PROGRESS = rgba_color(200, 100, 30, 255);
const color COLOR_SIDEBAR_BG = rgba_color(45, 45, 45, 255);
const color COLOR_SECTION_LABEL = rgba_color(160, 160, 160, 255);
const color COLOR_HINT_PLACEMENT = rgba_color(100, 180, 255, 255);

const color COLOR_CIRCUIT_SAVE_MSG = rgba_color(46, 184, 97, 255);

CircuitEditorMenu::CircuitEditorMenu(Circuit circuit, int window_width, int window_height, SettingsHandler& settings_handler)
    : circuit(std::move(circuit)),
      window_width(window_width),
      window_height(window_height),
      settings_handler(settings_handler),
      wire_start()
{
    print_info("CircuitEditorMenu created for circuit: " + this->circuit.circuit_name);
}

void CircuitEditorMenu::on_enter(WindowHandler &win_handler, MenuHandler &main_handler) {
    window_handler = &win_handler;
    menu_handler = &main_handler;
    show_grid = settings_handler.get_setting<bool>("showGrid");
    snap_to_grid = show_grid && settings_handler.get_setting<bool>("snapToGrid");
    print_info("Entered circuit editor: " + circuit.circuit_name);
}

float CircuitEditorMenu::canvas_width() const {
    return window_width - SIDEBAR_WIDTH;
}

float CircuitEditorMenu::snap_to_grid_value(float value) const {
    if (!snap_to_grid) {
        return value;
    }
    return std::round(value / GRID_CELL_SIZE) * GRID_CELL_SIZE;
}

point_2d CircuitEditorMenu::to_world_position(float screen_x, float screen_y) const {
    return {screen_x - pan_offset_x, screen_y - pan_offset_y};
}

void CircuitEditorMenu::handle_pan() {
    float mouse_x = mouse_position().x;
    float mouse_y = mouse_position().y;

    if (mouse_down(MIDDLE_BUTTON)) {
        if (!panning) {
            panning = true;
            pan_start_mouse_x = mouse_x;
            pan_start_mouse_y = mouse_y;
            pan_start_offset_x = pan_offset_x;
            pan_start_offset_y = pan_offset_y;
        } else {
            pan_offset_x = pan_start_offset_x + (mouse_x - pan_start_mouse_x);
            pan_offset_y = pan_start_offset_y + (mouse_y - pan_start_mouse_y);
        }
    } else {
        panning = false;
    }
}

void CircuitEditorMenu::handle_input() {
    circuit.simulate();

    if (save_feedback_timer > 0) {
        save_feedback_timer--;
    }

    handle_toolbar();

    if (key_typed(ESCAPE_KEY)) {
        if (drawing_wire) {
            cancel_wire();
        } else if (pending_placement != PendingPlacement::NONE) {
            pending_placement = PendingPlacement::NONE;
        }
        return;
    }

    handle_pan();
    handle_mouse();
}

void CircuitEditorMenu::handle_mouse() {
    float mouse_x = mouse_position().x;
    float mouse_y = mouse_position().y;

    bool in_toolbar = mouse_y < TOOLBAR_HEIGHT;
    bool in_sidebar = mouse_x >= canvas_width();

    point_2d world_position = to_world_position(mouse_x, mouse_y);

    if (mouse_clicked(RIGHT_BUTTON) && !in_toolbar && !in_sidebar) {
        bool shift_held = key_down(LEFT_SHIFT_KEY) || key_down(RIGHT_SHIFT_KEY);
        if (shift_held) {
            try_rename_at(world_position.x, world_position.y);
            return;
        }

        if (drawing_wire) {
            cancel_wire();
        } else if (pending_placement != PendingPlacement::NONE) {
            pending_placement = PendingPlacement::NONE;
        } else {
            try_remove_at(world_position.x, world_position.y);
        }
        return;
    }

    if (mouse_clicked(LEFT_BUTTON) && !in_toolbar && !in_sidebar) {
        if (pending_placement != PendingPlacement::NONE) {
            place_pending(world_position.x, world_position.y);
            return;
        }

        if (drawing_wire) {
            finish_wire(world_position.x, world_position.y);
            return;
        }

        bool shift_held = key_down(LEFT_SHIFT_KEY) || key_down(RIGHT_SHIFT_KEY);
        if (shift_held) {
            WireEndpoint clicked_pin;
            if (connectable_pin_at(world_position.x, world_position.y, clicked_pin)) {
                start_wire(world_position.x, world_position.y);
                return;
            }
        } else {
            int hit_input = input_pin_at(world_position.x, world_position.y);
            if (hit_input != -1) {
                circuit.toggle_input_pin(hit_input);
                play_sound_effect("click_success");
                return;
            }
        }
    }

    if (mouse_down(LEFT_BUTTON) && !mouse_left_just_pressed) {
        mouse_left_just_pressed = true;
        bool no_pending = pending_placement == PendingPlacement::NONE;
        if (!in_toolbar && !in_sidebar && drag_target == DragTarget::NONE && !drawing_wire && no_pending) {
            start_drag(world_position.x, world_position.y);
        }
    } else if (mouse_down(LEFT_BUTTON) && drag_target != DragTarget::NONE) {
        update_drag(mouse_x, mouse_y);
    }

    if (!mouse_down(LEFT_BUTTON)) {
        mouse_left_just_pressed = false;
        if (drag_target != DragTarget::NONE) {
            end_drag();
        }
    }
}

void CircuitEditorMenu::place_pending(float canvas_x, float canvas_y) {
    switch (pending_placement) {
        case PendingPlacement::GATE_AND:
            circuit.add_gate(GateType::AND, snap_to_grid_value(canvas_x - GATE_WIDTH / 2.0f), snap_to_grid_value(canvas_y - GATE_HEIGHT / 2.0f));
            break;
        case PendingPlacement::GATE_OR:
            circuit.add_gate(GateType::OR, snap_to_grid_value(canvas_x - GATE_WIDTH / 2.0f), snap_to_grid_value(canvas_y - GATE_HEIGHT / 2.0f));
            break;
        case PendingPlacement::GATE_NOT:
            circuit.add_gate(GateType::NOT, snap_to_grid_value(canvas_x - GATE_WIDTH / 2.0f), snap_to_grid_value(canvas_y - GATE_HEIGHT / 2.0f));
            break;
        case PendingPlacement::GATE_NAND:
            circuit.add_gate(GateType::NAND, snap_to_grid_value(canvas_x - GATE_WIDTH / 2.0f), snap_to_grid_value(canvas_y - GATE_HEIGHT / 2.0f));
            break;
        case PendingPlacement::GATE_NOR:
            circuit.add_gate(GateType::NOR, snap_to_grid_value(canvas_x - GATE_WIDTH / 2.0f), snap_to_grid_value(canvas_y - GATE_HEIGHT / 2.0f));
            break;
        case PendingPlacement::GATE_XOR:
            circuit.add_gate(GateType::XOR, snap_to_grid_value(canvas_x - GATE_WIDTH / 2.0f), snap_to_grid_value(canvas_y - GATE_HEIGHT / 2.0f));
            break;
        case PendingPlacement::GATE_XNOR:
            circuit.add_gate(GateType::XNOR, snap_to_grid_value(canvas_x - GATE_WIDTH / 2.0f), snap_to_grid_value(canvas_y - GATE_HEIGHT / 2.0f));
            break;
        case PendingPlacement::INPUT_PIN:
            circuit.add_input_pin(snap_to_grid_value(canvas_x), snap_to_grid_value(canvas_y));
            break;
        case PendingPlacement::OUTPUT_PIN:
            circuit.add_output_pin(snap_to_grid_value(canvas_x), snap_to_grid_value(canvas_y));
            break;
        case PendingPlacement::NONE:
            break;
    }
    play_sound_effect("click_success");
    pending_placement = PendingPlacement::NONE;
}

void CircuitEditorMenu::start_drag(float world_x, float world_y) {
    int hit_gate = gate_at(world_x, world_y);
    if (hit_gate != -1) {
        Gate *gate = circuit.find_gate_with_id(hit_gate);
        drag_target = DragTarget::GATE;
        dragged_id = hit_gate;
        drag_offset_x = world_x - gate->x_position;
        drag_offset_y = world_y - gate->y_position;
        return;
    }

    int hit_input = input_pin_at(world_x, world_y);
    if (hit_input != -1) {
        InputPin *pin = circuit.find_input_pin_by_id(hit_input);
        drag_target = DragTarget::INPUT_PIN;
        dragged_id = hit_input;
        drag_offset_x = world_x - pin->x_position;
        drag_offset_y = world_y - pin->y_position;
        return;
    }

    int hit_output = output_pin_at(world_x, world_y);
    if (hit_output != -1) {
        OutputPin *pin = circuit.find_output_pin_by_id(hit_output);
        drag_target = DragTarget::OUTPUT_PIN;
        dragged_id = hit_output;
        drag_offset_x = world_x - pin->x_position;
        drag_offset_y = world_y - pin->y_position;
    }
}

void CircuitEditorMenu::update_drag(float mouse_x, float mouse_y) {
    point_2d world = to_world_position(mouse_x, mouse_y);
    float new_x = snap_to_grid_value(world.x - drag_offset_x);
    float new_y = snap_to_grid_value(world.y - drag_offset_y);

    if (drag_target == DragTarget::GATE) {
        new_x = std::min(new_x, canvas_width() - GATE_WIDTH);
        new_y = std::max(new_y, TOOLBAR_HEIGHT);
        Gate *gate = circuit.find_gate_with_id(dragged_id);
        if (gate) {
            gate->x_position = new_x;
            gate->y_position = new_y;
        }
    } else if (drag_target == DragTarget::INPUT_PIN) {
        new_x = std::min(new_x, canvas_width() - STANDALONE_PIN_RADIUS);
        new_x = std::max(new_x, STANDALONE_PIN_RADIUS);
        new_y = std::max(new_y, TOOLBAR_HEIGHT + STANDALONE_PIN_RADIUS);
        InputPin *pin = circuit.find_input_pin_by_id(dragged_id);
        if (pin) {
            pin->x_position = new_x;
            pin->y_position = new_y;
        }
    } else if (drag_target == DragTarget::OUTPUT_PIN) {
        new_x = std::min(new_x, canvas_width() - STANDALONE_PIN_RADIUS);
        new_x = std::max(new_x, STANDALONE_PIN_RADIUS);
        new_y = std::max(new_y, TOOLBAR_HEIGHT + STANDALONE_PIN_RADIUS);
        OutputPin *pin = circuit.find_output_pin_by_id(dragged_id);
        if (pin) {
            pin->x_position = new_x;
            pin->y_position = new_y;
        }
    }
}

void CircuitEditorMenu::end_drag() {
    drag_target = DragTarget::NONE;
    dragged_id = -1;
}

void CircuitEditorMenu::start_wire(float mouse_x, float mouse_y) {
    WireEndpoint clicked_pin;
    if (!connectable_pin_at(mouse_x, mouse_y, clicked_pin)) {
        return;
    }
    drawing_wire = true;
    wire_start = clicked_pin;
    print_info("Wire drawing started");
}

void CircuitEditorMenu::finish_wire(float mouse_x, float mouse_y) {
    WireEndpoint wire_end;
    if (!connectable_pin_at(mouse_x, mouse_y, wire_end)) {
        cancel_wire();
        return;
    }

    if (!is_valid_wire_connection(wire_start, wire_end)) {
        print_warning("Invalid wire connection attempted");
        cancel_wire();
        return;
    }

    WireEndpoint starting_source = is_output_side(wire_start) ? wire_start : wire_end;
    WireEndpoint ending_source = is_output_side(wire_start) ? wire_end : wire_start;

    circuit.add_wire(
        starting_source.object_type, starting_source.object_id, starting_source.pin_id,
        ending_source.object_type, ending_source.object_id, ending_source.pin_id
    );

    play_sound_effect("click_success");

    print_info("Wire added");
    drawing_wire = false;
}

void CircuitEditorMenu::cancel_wire() {
    drawing_wire = false;
    play_sound_effect("click_success");
    print_info("Wire drawing cancelled");
}

bool CircuitEditorMenu::is_output_side(const WireEndpoint &endpoint) const {
    if (endpoint.object_type == WireConnectionType::INPUT_PIN) {
        return true;
    }

    if (endpoint.object_type == WireConnectionType::OUTPUT_PIN) {
        return false;
    }

    for (const Gate &gate : circuit.circuit_gates) {
        if (gate.id != endpoint.object_id) {
            continue;
        }
        for (const Pin &pin : gate.output_pins) {
            if (pin.pin_id == endpoint.pin_id) {
                return true;
            }
        }
        return false;
    }

    return false;
}

bool CircuitEditorMenu::is_valid_wire_connection(
    const WireEndpoint &from,
    const WireEndpoint &to
) const {
    if (from.object_type == to.object_type &&
        from.object_id == to.object_id &&
        from.pin_id == to.pin_id
    ) {
        return false;
    }

    bool from_is_output = is_output_side(from);
    bool to_is_output = is_output_side(to);

    if (from_is_output == to_is_output) {
        return false;
    }

    const WireEndpoint &input_side = from_is_output ? to : from;

    for (const Wire &wire_to_check : circuit.circuit_wires) {
        bool set_input_already = (
            wire_to_check.to_type == input_side.object_type &&
            wire_to_check.to_id == input_side.object_id &&
            wire_to_check.to_pin_id == input_side.pin_id
        );

        if (set_input_already) {
            return false;
        }
    }

    for (const Wire &wire_to_check : circuit.circuit_wires) {
        bool same_forward = (
            wire_to_check.from_type == from.object_type &&
            wire_to_check.from_id == from.object_id &&
            wire_to_check.from_pin_id == from.pin_id &&
            wire_to_check.to_type == to.object_type &&
            wire_to_check.to_id == to.object_id &&
            wire_to_check.to_pin_id == to.pin_id
        );

        bool same_backward = (
            wire_to_check.from_type == to.object_type &&
            wire_to_check.from_id == to.object_id &&
            wire_to_check.from_pin_id == to.pin_id &&
            wire_to_check.to_type == from.object_type &&
            wire_to_check.to_id == from.object_id &&
            wire_to_check.to_pin_id == from.pin_id
        );

        if (same_forward || same_backward) {
            return false;
        }
    }

    return true;
}

void CircuitEditorMenu::try_remove_at(float mouse_x, float mouse_y) {
    int hit_gate = gate_at(mouse_x, mouse_y);
    if (hit_gate != -1) {
        circuit.remove_gate(hit_gate);
        play_sound_effect("click_success");
        return;
    }

    int hit_input = input_pin_at(mouse_x, mouse_y);
    if (hit_input != -1) {
        circuit.remove_input_pin(hit_input);
        play_sound_effect("click_success");
        return;
    }

    int hit_output = output_pin_at(mouse_x, mouse_y);
    if (hit_output != -1) {
        circuit.remove_output_pin(hit_output);
        play_sound_effect("click_success");
        return;
    }

    int hit_wire = wire_at(mouse_x, mouse_y);
    if (hit_wire != -1) {
        circuit.remove_wire(hit_wire);
    }
}

void CircuitEditorMenu::try_rename_at(float world_x, float world_y) {
    int hit_input = input_pin_at(world_x, world_y);
    if (hit_input != -1) {
        InputPin* pin = circuit.find_input_pin_by_id(hit_input);
        if (!pin) {
            return;
        }

        std::string current_label = pin->label;
        menu_handler->push(std::make_unique<RenameModal>(
            "Rename Input Pin",
            current_label,
            [this, hit_input](const std::string& new_label) {
                InputPin* found_pin = circuit.find_input_pin_by_id(hit_input);
                if (found_pin) {
                    found_pin->label = new_label;
                }
            }
        ));
        return;
    }

    int hit_output = output_pin_at(world_x, world_y);
    if (hit_output != -1) {
        OutputPin* pin = circuit.find_output_pin_by_id(hit_output);
        if (!pin) {
            return;
        }

        std::string current_label = pin->label;
        menu_handler->push(std::make_unique<RenameModal>(
            "Rename Output Pin",
            current_label,
            [this, hit_output](const std::string& new_label) {
                OutputPin* found_pin = circuit.find_output_pin_by_id(hit_output);
                if (found_pin) {
                    found_pin->label = new_label;
                }
            }
        ));
    }
}

int CircuitEditorMenu::gate_at(float x, float y) const {
    for (const Gate &gate : circuit.circuit_gates) {
        bool is_in_x = x >= gate.x_position && x <= gate.x_position + GATE_WIDTH;
        bool is_in_y = y >= gate.y_position && y <= gate.y_position + GATE_HEIGHT;
        if (is_in_x && is_in_y) {
            return gate.id;
        }
    }
    return -1;
}

int CircuitEditorMenu::input_pin_at(float x, float y) const {
    for (const InputPin &pin : circuit.circuit_input_pins) {
        float delta_x = x - pin.x_position;
        float delta_y = y - pin.y_position;
        if (delta_x * delta_x + delta_y * delta_y <= STANDALONE_PIN_RADIUS * STANDALONE_PIN_RADIUS) {
            return pin.id;
        }
    }
    return -1;
}

int CircuitEditorMenu::output_pin_at(float x, float y) const {
    for (const OutputPin &pin : circuit.circuit_output_pins) {
        float delta_x = x - pin.x_position;
        float delta_y = y - pin.y_position;
        if (delta_x * delta_x + delta_y * delta_y <= STANDALONE_PIN_RADIUS * STANDALONE_PIN_RADIUS) {
            return pin.id;
        }
    }
    return -1;
}

bool CircuitEditorMenu::gate_pin_at(float x, float y, WireEndpoint &out_endpoint) const {
    for (const Gate &gate : circuit.circuit_gates) {
        int input_count = (int)(gate.input_pins.size());
        float slot_height = GATE_HEIGHT / (input_count + 1);
        for (int i = 0; i < input_count; i++) {
            float pin_x_position = gate.x_position;
            float pin_y_position = gate.y_position + slot_height * (i + 1);

            float delta_x = x - pin_x_position;
            float delta_y = y - pin_y_position;

            if (delta_x * delta_x + delta_y * delta_y <= GATE_PIN_HIT_RADIUS * GATE_PIN_HIT_RADIUS) {
                out_endpoint = {WireConnectionType::GATE, gate.id, gate.input_pins[i].pin_id};
                return true;
            }
        }

        float out_x = gate.x_position + GATE_WIDTH;
        float out_y = gate.y_position + (GATE_HEIGHT / 2.0f);

        float delta_x = x - out_x;
        float delta_y = y - out_y;

        if (delta_x * delta_x + delta_y * delta_y <= GATE_PIN_HIT_RADIUS * GATE_PIN_HIT_RADIUS) {
            out_endpoint = {WireConnectionType::GATE, gate.id, gate.output_pins[0].pin_id};
            return true;
        }
    }
    return false;
}

bool CircuitEditorMenu::connectable_pin_at(float x, float y, WireEndpoint &out_endpoint) const {
    if (gate_pin_at(x, y, out_endpoint)) {
        return true;
    }

    for (const InputPin &pin : circuit.circuit_input_pins) {
        float delta_x = x - pin.x_position;
        float delta_y = y - pin.y_position;
        if (delta_x * delta_x + delta_y * delta_y <= STANDALONE_PIN_RADIUS * STANDALONE_PIN_RADIUS) {
            out_endpoint = {WireConnectionType::INPUT_PIN, pin.id, 0};
            return true;
        }
    }

    for (const OutputPin &pin : circuit.circuit_output_pins) {
        float delta_x = x - pin.x_position;
        float delta_y = y - pin.y_position;
        if (delta_x * delta_x + delta_y * delta_y <= STANDALONE_PIN_RADIUS * STANDALONE_PIN_RADIUS) {
            out_endpoint = {WireConnectionType::OUTPUT_PIN, pin.id, 0};
            return true;
        }
    }

    return false;
}

int CircuitEditorMenu::wire_at(float x, float y) const {
    for (const Wire &wire : circuit.circuit_wires) {
        point_2d from_point = connection_point_for(wire.from_type, wire.from_id, wire.from_pin_id);
        point_2d to_point = connection_point_for(wire.to_type, wire.to_id, wire.to_pin_id);

        float wire_dx = to_point.x - from_point.x;
        float wire_dy = to_point.y - from_point.y;
        float wire_length_sq = wire_dx * wire_dx + wire_dy * wire_dy;

        float segment_percent = 0.0f;
        if (wire_length_sq > 0.0001f) {
            segment_percent = ((x - from_point.x) * wire_dx + (y - from_point.y) * wire_dy) / wire_length_sq;
            segment_percent = std::max(0.0f, std::min(1.0f, segment_percent));
        }

        float closest_x = from_point.x + segment_percent * wire_dx;
        float closest_y = from_point.y + segment_percent * wire_dy;

        float dist_x = x - closest_x;
        float dist_y = y - closest_y;
        float distance = std::sqrt(dist_x * dist_x + dist_y * dist_y);

        if (distance <= WIRE_HIT_TOLERANCE) {
            return wire.id;
        }
    }
    return -1;
}

void CircuitEditorMenu::draw() const {
    float mouse_x = mouse_position().x;
    float mouse_y = mouse_position().y;

    draw_grid();
    draw_wires();
    if (drawing_wire) {
        draw_wire_in_progress(mouse_x, mouse_y);
    }
    draw_gates();
    draw_input_pins();
    draw_output_pins();
    draw_sidebar();
    draw_toolbar();
}

void CircuitEditorMenu::draw_grid() const {
    if (!show_grid) {
        return;
    }
    float grid_top = TOOLBAR_HEIGHT;
    color grid_color = rgba_color(210, 210, 210, 255);

    float offset_x = std::fmod(pan_offset_x, GRID_CELL_SIZE);
    float offset_y = std::fmod(pan_offset_y, GRID_CELL_SIZE);

    for (float x = offset_x; x < canvas_width(); x += GRID_CELL_SIZE) {
        draw_line(grid_color, x, grid_top, x, window_height);
    }
    for (float y = grid_top + offset_y; y < window_height; y += GRID_CELL_SIZE) {
        draw_line(grid_color, 0, y, canvas_width(), y);
    }
}

void CircuitEditorMenu::handle_toolbar() {
    bool clicked_back = draw_back_button();
    if (clicked_back) {
        play_sound_effect("ui_click");
        menu_handler->pop();
        return;
    }

    float save_button_width = 80.0f;
    float edit_button_width = 80.0f;

    float button_height = 36.0f;
    float button_y = (TOOLBAR_HEIGHT - button_height) / 2.0f;

    float save_button_x = canvas_width() - save_button_width - 12.0f;
    float edit_button_x = save_button_x - edit_button_width - 8.0f;

    bool save_clicked = unique_button("Save", rectangle_from(save_button_x, button_y, save_button_width, button_height));
    if (save_clicked) {
        play_sound_effect("ui_click");
        CircuitFileHandler file_handler;
        bool saved = file_handler.save_circuit(circuit);
        if (saved) {
            print_info("Circuit saved from editor");
            save_feedback_timer = 180;
        } else {
            print_error("Failed to save circuit from editor");
        }
    }

    bool edit_clicked = unique_button("Edit", rectangle_from(edit_button_x, button_y, edit_button_width, button_height));
    if (edit_clicked) {
        play_sound_effect("ui_click");
        menu_handler->push(std::make_unique<EditCircuitDetailsMenu>(
            circuit.circuit_name,
            circuit.circuit_description,
            circuit.circuit_name,
            [this](const std::string& new_name, const std::string& new_desc) {
                CircuitFileHandler file_handler;
                if (circuit.circuit_name != new_name) {
                    file_handler.delete_circuit(circuit.circuit_name);
                }
                circuit.circuit_name = new_name;
                circuit.circuit_description = new_desc;
                file_handler.save_circuit(circuit);
                save_feedback_timer = 180;
            }
        ));
    }
}

void CircuitEditorMenu::draw_toolbar() const {
    fill_rectangle(COLOR_LIGHT_GRAY, 0, 0, window_width, TOOLBAR_HEIGHT);

    int name_font_size = 20;
    float name_x = (canvas_width() / 2.0f) - (text_width(circuit.circuit_name, EDITOR_FONT, name_font_size) / 2.0f);
    draw_text(circuit.circuit_name, COLOR_BLACK, EDITOR_FONT, name_font_size, name_x, 12.0f);

    if (!circuit.circuit_description.empty()) {
        int desc_font_size = 13;
        color desc_color = rgba_color(90, 90, 90, 255);
        float desc_x = (canvas_width() / 2.0f) - (text_width(circuit.circuit_description, EDITOR_FONT, desc_font_size) / 2.0f);
        draw_text(circuit.circuit_description, desc_color, EDITOR_FONT, desc_font_size, desc_x, 38.0f);
    }
}


void CircuitEditorMenu::draw_sidebar() const {
    float sidebar_x = canvas_width();
    fill_rectangle(COLOR_SIDEBAR_BG, sidebar_x, 0, SIDEBAR_WIDTH, window_height);

    float button_x = sidebar_x + SIDEBAR_PADDING;
    float button_width = SIDEBAR_WIDTH - SIDEBAR_PADDING * 2;
    float current_y = TOOLBAR_HEIGHT + SIDEBAR_PADDING;

    draw_text("GATES", COLOR_SECTION_LABEL, EDITOR_FONT, 12, button_x, current_y);
    current_y += 20.0f;

    const std::vector<std::pair<std::string, PendingPlacement>> gate_buttons = {
        {"AND", PendingPlacement::GATE_AND},
        {"OR", PendingPlacement::GATE_OR},
        {"NOT", PendingPlacement::GATE_NOT},
        {"NAND", PendingPlacement::GATE_NAND},
        {"NOR", PendingPlacement::GATE_NOR},
        {"XOR", PendingPlacement::GATE_XOR},
        {"XNOR", PendingPlacement::GATE_XNOR}
    };

    for (const auto &[label, placement] : gate_buttons) {
        bool clicked = unique_button(label, rectangle_from(button_x, current_y, button_width, SIDEBAR_BUTTON_HEIGHT));
        if (clicked) {
            play_sound_effect("ui_click");
            pending_placement = placement;
        }
        current_y += SIDEBAR_BUTTON_HEIGHT + SIDEBAR_BUTTON_GAP;
    }

    current_y += 4.0f;
    draw_text("PINS", COLOR_SECTION_LABEL, EDITOR_FONT, 12, button_x, current_y);
    current_y += 20.0f;

    bool input_clicked = unique_button("Input Pin", rectangle_from(button_x, current_y, button_width, SIDEBAR_BUTTON_HEIGHT));
    if (input_clicked) {
        play_sound_effect("ui_click");
        pending_placement = PendingPlacement::INPUT_PIN;
    }
    current_y += SIDEBAR_BUTTON_HEIGHT + SIDEBAR_BUTTON_GAP;

    bool output_clicked = unique_button("Output Pin", rectangle_from(button_x, current_y, button_width, SIDEBAR_BUTTON_HEIGHT));
    if (output_clicked) {
        play_sound_effect("ui_click");
        pending_placement = PendingPlacement::OUTPUT_PIN;
    }

    if (drawing_wire) {
        draw_text("Click any pin", COLOR_WIRE_IN_PROGRESS, EDITOR_FONT, 11, button_x, window_height - 56.0f);
        draw_text("to finish wire", COLOR_WIRE_IN_PROGRESS, EDITOR_FONT, 11, button_x, window_height - 42.0f);
        draw_text("Esc / R-click", COLOR_WIRE_IN_PROGRESS, EDITOR_FONT, 11, button_x, window_height - 28.0f);
        draw_text("to cancel", COLOR_WIRE_IN_PROGRESS, EDITOR_FONT, 11, button_x, window_height - 14.0f);
    } else if (pending_placement != PendingPlacement::NONE) {
        draw_text("Click the canvas", COLOR_HINT_PLACEMENT, EDITOR_FONT, 11, button_x, window_height - 56.0f);
        draw_text("to place item", COLOR_HINT_PLACEMENT, EDITOR_FONT, 11, button_x, window_height - 42.0f);
        draw_text("Esc / R-click", COLOR_HINT_PLACEMENT, EDITOR_FONT, 11, button_x, window_height - 28.0f);
        draw_text("to cancel", COLOR_HINT_PLACEMENT, EDITOR_FONT, 11, button_x, window_height - 14.0f);
    } else if (save_feedback_timer > 0) {
        draw_text("Circuit saved!", COLOR_CIRCUIT_SAVE_MSG, EDITOR_FONT, 11, button_x, window_height - 28.0f);
    } else {
        draw_text("Shift+L-click pin", COLOR_SECTION_LABEL, EDITOR_FONT, 11, button_x, window_height - 112.0f);
        draw_text("to start a wire", COLOR_SECTION_LABEL, EDITOR_FONT, 11, button_x, window_height - 98.0f);
        draw_text("Shift+R-click pin", COLOR_SECTION_LABEL, EDITOR_FONT, 11, button_x, window_height - 70.0f);
        draw_text("to rename pin", COLOR_SECTION_LABEL, EDITOR_FONT, 11, button_x, window_height - 56.0f);
        draw_text("Middle mouse button", COLOR_SECTION_LABEL, EDITOR_FONT, 11, button_x, window_height - 28.0f);
        draw_text("to pan canvas", COLOR_SECTION_LABEL, EDITOR_FONT, 11, button_x, window_height - 14.0f);
    }
}

void CircuitEditorMenu::draw_gates() const {
    for (const Gate &gate : circuit.circuit_gates) {
        draw_gate(gate);
    }
}

void CircuitEditorMenu::draw_gate(const Gate &gate) const {
    float gate_x_position = gate.x_position + pan_offset_x;
    float gate_y_position = gate.y_position + pan_offset_y;

    bool is_dragged = (drag_target == DragTarget::GATE && dragged_id == gate.id);
    color body_color = is_dragged ? dragged_gate_color_for_type(gate.gate_type) : gate_color_for_type(gate.gate_type);

    fill_rectangle(body_color, gate_x_position, gate_y_position, GATE_WIDTH, GATE_HEIGHT);
    draw_rectangle(COLOR_GATE_OUTLINE, gate_x_position, gate_y_position, GATE_WIDTH, GATE_HEIGHT);

    std::string label = gate_type_to_string(gate.gate_type);
    int font_size = 16;
    float label_x = gate_x_position + (GATE_WIDTH / 2.0f) - (text_width(label, EDITOR_FONT, font_size) / 2.0f);
    float label_y = gate_y_position + (GATE_HEIGHT / 2.0f) - (font_size / 2.0f);
    draw_text(label, COLOR_GATE_LABEL, EDITOR_FONT, font_size, label_x, label_y);

    int input_count = (int)(gate.input_pins.size());
    float slot_height = GATE_HEIGHT / (input_count + 1);
    for (int i = 0; i < input_count; i++) {
        float pin_x = gate_x_position;
        float pin_y = gate_y_position + slot_height * (i + 1);
        color pin_color = gate.input_pins[i].value ? COLOR_PIN_HIGH : COLOR_PIN_LOW;
        fill_circle(pin_color, pin_x, pin_y, GATE_PIN_RADIUS);
        draw_circle(COLOR_GATE_OUTLINE, pin_x, pin_y, GATE_PIN_RADIUS);
    }

    float out_x = gate_x_position + GATE_WIDTH;
    float out_y = gate_y_position + (GATE_HEIGHT / 2.0f);
    color out_pin_color = gate.output_pins.empty() ? COLOR_PIN_LOW : (gate.output_pins[0].value ? COLOR_PIN_HIGH : COLOR_PIN_LOW);
    fill_circle(out_pin_color, out_x, out_y, GATE_PIN_RADIUS);
    draw_circle(COLOR_GATE_OUTLINE, out_x, out_y, GATE_PIN_RADIUS);
}

void CircuitEditorMenu::draw_input_pins() const {
    for (const InputPin &pin : circuit.circuit_input_pins) {
        draw_input_pin(pin);
    }
}

void CircuitEditorMenu::draw_input_pin(const InputPin &pin) const {
    bool is_dragged = (drag_target == DragTarget::INPUT_PIN && dragged_id == pin.id);
    color body_color = is_dragged ? COLOR_PIN_DRAGGED : (pin.value ? COLOR_PIN_HIGH : COLOR_PIN_LOW);

    float screen_x = pin.x_position + pan_offset_x;
    float screen_y = pin.y_position + pan_offset_y;

    fill_circle(body_color, screen_x, screen_y, STANDALONE_PIN_RADIUS);
    draw_circle(COLOR_PIN_OUTLINE, screen_x, screen_y, STANDALONE_PIN_RADIUS);

    std::string label = pin.label.empty() ? "IN" : pin.label;
    int font_size = 12;
    float label_x = screen_x - (text_width(label, EDITOR_FONT, font_size) / 2.0f);
    float label_y = screen_y + STANDALONE_PIN_RADIUS + 2.0f;
    draw_text(label, COLOR_PIN_LABEL, EDITOR_FONT, font_size, label_x, label_y);
}

void CircuitEditorMenu::draw_output_pins() const {
    for (const OutputPin &pin : circuit.circuit_output_pins) {
        draw_output_pin(pin);
    }
}

void CircuitEditorMenu::draw_output_pin(const OutputPin &pin) const {
    bool is_dragged = (drag_target == DragTarget::OUTPUT_PIN && dragged_id == pin.id);
    color body_color = is_dragged ? COLOR_PIN_DRAGGED : (pin.value ? COLOR_PIN_HIGH : COLOR_PIN_LOW);

    float screen_x = pin.x_position + pan_offset_x;
    float screen_y = pin.y_position + pan_offset_y;

    fill_circle(body_color, screen_x, screen_y, STANDALONE_PIN_RADIUS);
    draw_circle(COLOR_PIN_OUTLINE, screen_x, screen_y, STANDALONE_PIN_RADIUS);

    std::string label = pin.label.empty() ? "OUT" : pin.label;
    int font_size = 12;
    float label_x = screen_x - (text_width(label, EDITOR_FONT, font_size) / 2.0f);
    float label_y = screen_y + STANDALONE_PIN_RADIUS + 2.0f;
    draw_text(label, COLOR_PIN_LABEL, EDITOR_FONT, font_size, label_x, label_y);
}

void CircuitEditorMenu::draw_wires() const {
    for (const Wire &wire : circuit.circuit_wires) {
        draw_wire(wire);
    }
}

void CircuitEditorMenu::draw_wire(const Wire &wire) const {
    point_2d from_world = connection_point_for(wire.from_type, wire.from_id, wire.from_pin_id);
    point_2d to_world = connection_point_for(wire.to_type, wire.to_id, wire.to_pin_id);

    float from_start_x = from_world.x + pan_offset_x;
    float from_start_y = from_world.y + pan_offset_y;
    float to_start_x = to_world.x + pan_offset_x;
    float to_start_y = to_world.y + pan_offset_y;

    bool wire_is_high = false;

    if (wire.from_type == WireConnectionType::INPUT_PIN) {
        for (const InputPin &pin : circuit.circuit_input_pins) {
            if (pin.id == wire.from_id) {
                wire_is_high = pin.value;
                break;
            }
        }
    } else if (wire.from_type == WireConnectionType::GATE) {
        for (const Gate &gate : circuit.circuit_gates) {
            if (gate.id != wire.from_id) continue;
            for (const Pin &output_pin : gate.output_pins) {
                if (output_pin.pin_id == wire.from_pin_id) {
                    wire_is_high = output_pin.value;
                    break;
                }
            }
            break;
        }
    } else if (wire.from_type == WireConnectionType::OUTPUT_PIN) {
        for (const OutputPin &pin : circuit.circuit_output_pins) {
            if (pin.id == wire.from_id) {
                wire_is_high = pin.value;
                break;
            }
        }
    }

    color wire_color = wire_is_high ? COLOR_WIRE_HIGH : COLOR_WIRE;
    draw_line(wire_color, from_start_x, from_start_y, to_start_x, to_start_y, option_line_width(3));
}

void CircuitEditorMenu::draw_wire_in_progress(float mouse_x, float mouse_y) const {
    point_2d start_world = connection_point_for(
        wire_start.object_type,
        wire_start.object_id,
        wire_start.pin_id
    );

    float start_x = start_world.x + pan_offset_x;
    float start_y = start_world.y + pan_offset_y;
    draw_line(COLOR_WIRE_IN_PROGRESS, start_x, start_y, mouse_x, mouse_y, option_line_width(3));
    draw_circle(COLOR_WIRE_IN_PROGRESS, start_x, start_y, GATE_PIN_RADIUS + 2.0f);
}

point_2d CircuitEditorMenu::connection_point_for(
    WireConnectionType type,
    int object_id,
    int pin_id
) const {
    if (type == WireConnectionType::INPUT_PIN) {
        for (const InputPin &pin : circuit.circuit_input_pins) {
            if (pin.id == object_id) {
                return {pin.x_position, pin.y_position};
            }
        }
    }

    if (type == WireConnectionType::OUTPUT_PIN) {
        for (const OutputPin &pin : circuit.circuit_output_pins) {
            if (pin.id == object_id) {
                return {pin.x_position, pin.y_position};
            }
        }
    }

    if (type == WireConnectionType::GATE) {
        for (const Gate &gate : circuit.circuit_gates) {
            if (gate.id != object_id) {
                continue;
            }

            for (const Pin &p : gate.output_pins) {
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

    print_warning("connection_point_for() could not calculate connection point for wire");
    return {0.0f, 0.0f};
}