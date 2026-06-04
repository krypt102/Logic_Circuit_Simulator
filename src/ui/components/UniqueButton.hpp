#ifndef LOGIC_CIRCUIT_SIMULATOR_UNIQUE_BUTTON_HPP
#define LOGIC_CIRCUIT_SIMULATOR_UNIQUE_BUTTON_HPP

#include "splashkit.h"
#include <string>

void reset_button_counter();
bool unique_button(const std::string& label, const rectangle &rect);

#endif // LOGIC_CIRCUIT_SIMULATOR_UNIQUE_BUTTON_HPP