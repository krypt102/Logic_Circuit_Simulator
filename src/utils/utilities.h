#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
using std::string;

/**
 * Print a string of text a repeated amount of times
 *
 * @param text_to_print The text to be printed
 * @param times_to_print The amount of times to repeat
 * @param with_newline Whether to add a newline
 */
void print_repeated(string text_to_print, int times_to_print, bool with_newline);

/**
 * Print a dash character a repeated amount of times
 *
 * @param line_length The length of the dashed line (characters)
 * @param with_newline Whether to add a newline
 */
void print_line(int line_length, bool with_newline);

/**
 * Read a string from the user
 *
 * @param prompt the message to show the user
 * @returns the string entered
 */
string read_string(string prompt);

/**
 * Read an integer from the user
 *
 * @param prompt the message to show the user
 * @returns the integer entered
 */
int read_integer(string prompt);

/**
 * Read an integer from the user, and only allows an integer within a range
 *
 * @param prompt the message to show the user
 * @param range_min the minimum integer to accept
 * @param range_max the maximum integer to accept
 * @returns the integer entered
 */
int read_integer_range(string prompt, int range_min, int range_max);

/**
 * Read a double from the user
 *
 * @param prompt the message to show the user
 * @returns the double entered
 */
double read_double(string prompt);

/**
 * Read a double from the user, and only allows an input within a range
 *
 * @param prompt the message to show the user
 * @param range_min the minimum integer to accept
 * @param range_max the maximum integer to accept
 * @returns the integer entered
 */
double read_double_range(string prompt, double range_min, double range_max);

/**
 *Reads user input from the terminal, but only allows two options - Y/N
 *
 *@param prompt the message to show the user
 *@returns the selected option
*/
string read_line_binary(string prompt);

#endif