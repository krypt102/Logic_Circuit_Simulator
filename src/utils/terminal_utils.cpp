#include <format>
#include "splashkit.h"

const string RESET_TXT = "\033[0m";
const string INFO_TXT = "\033[97m";
const string WARN_TXT = "\033[93m";
const string ERROR_TXT = "\033[91m";

namespace {
    void print_colored(const string &color, const string &label, const string &message) {
        write_line(std::format("{}[{}]: {}{}", color, label, message, RESET_TXT));
    }
}

void print_info(const string &message) {
    print_colored(INFO_TXT, "INFO", message);
}

void print_warning(const string &message) {
    print_colored(WARN_TXT, "WARNING", message);
}

void print_error(const string &message) {
    print_colored(ERROR_TXT, "ERROR", message);
}