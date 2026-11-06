// author: Jacky.Lin
// date: 2021/8/31
// description: parse command line parameters
#include <string>
#include <unordered_map>

class argumentParser {
    std::unordered_map<std::string, std::string> parameters;
public:
    argumentParser();
    argumentParser(const argumentParser& other);
    argumentParser(const int& argc, char* argv[], void (*usage)(int argc, char* argv[]));

    bool has(const std::string& key);
    const std::string get(const std::string& key);
};