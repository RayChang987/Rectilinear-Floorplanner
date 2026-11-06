#include "io_handler_t.h"

#include <iomanip>
#include <iostream>
using std::cout, std::cin, std::endl;
using std::ostringstream, std::fixed, std::setprecision;

string io_handler_t::input_file_name = "/outputs";
string io_handler_t::output_checker_file_name = ".";
string logger_t::to_string_with_precision(double value, int precision) {
    // Use a stringstream to format the double value with the specified
    // precision
    ostringstream out;
    out << fixed << setprecision(precision) << value;
    return out.str();
}
void logger_t::warning(vector<string> msgs) {
    for (string& msg : msgs) {
        string formatted_msg = "\033[1;33m [Warning] " + msg +
                               " \033[0m\n";  // Yellow color for warnings
        cout << formatted_msg << endl;
    }
}

void logger_t::result(vector<string> msgs) {
    for (string& msg : msgs) {
        string formatted_msg = "\033[1;34m [Result]  " + msg +
                               " \033[0m\n";  // Blue color for results
        cout << formatted_msg << endl;
    }
}

void logger_t::error(vector<string> msgs) {
    for (string& msg : msgs) {
        string formatted_msg = "\033[1;31m [Error]  " + msg +
                               "  \033[0m\n";  // Red color for errors
        cout << formatted_msg << endl;
    }
}

void logger_t::success(vector<string> msgs) {
    for (string& msg : msgs) {
        string formatted_msg =
            "\033[1;32m [Success] " + msg +
            " \033[0m\n";  // Green color for success messages
        cout << formatted_msg << endl;
    }
}

void logger_t::info(vector<string> msgs) {  // Define the info function
    for (string& msg : msgs) {
        string formatted_msg = "\033[1;34m [Info]    " + msg +
                               " \033[0m\n";  // Blue color for info
        cout << formatted_msg << endl;
    }
}

void logger_t::action(vector<string> msgs) {  // Define the actions function
    for (string& msg : msgs) {
        string formatted_msg = "\033[1;30m [Actions]  " + msg +
                               " \033[0m\n";  // Gray color for actions
        cout << formatted_msg << endl;
    }
}
