#ifndef INC_2023PD_IO_HANDLER_H
#define INC_2023PD_IO_HANDLER_H

#include <string>
#include <vector>
using std::string;
using std::vector;
class io_handler_t {
   public:
    static string input_file_name;
    static string output_checker_file_name;
};
class logger_t {
   public:
    static void warning(vector<string>);
    static void result(vector<string>);
    static void error(vector<string>);
    static void success(vector<string>);
    static void info(vector<string>);
    static void action(vector<string>);
    static string to_string_with_precision(double value, int precision);
};
#endif
