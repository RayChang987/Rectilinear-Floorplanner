#include "argumentParser.h"
#include <cstdint>
#include <stdexcept>
#include <cstring>

argumentParser::argumentParser(){
    this->parameters = std::unordered_map<std::string, std::string>();
}

argumentParser::argumentParser(const argumentParser &other){
    this->parameters = std::unordered_map<std::string, std::string>(other.parameters);
}

argumentParser::argumentParser(const int& argc, char* argv[], void (*usage)(int, char*[])) {
    for (int argIndex = 1; argIndex < argc; ) {
        
        // if the argument is not a flag, then print the usage and throw an exception
        if(argv[argIndex][0] != '-'){
            usage(argc, argv);
            std::string error = "Invalid argument at arg[" + std::to_string(argIndex) + "] : " + argv[argIndex];
            throw std::invalid_argument(error.c_str());
        }

        // when the flag is long string (--<string> <arg>), substr the second char 
        // when the flag is char (-<char> <arg>), substr the first char
        uint8_t substrLen = 0;
        substrLen = (strncmp(argv[argIndex], "--", 2)) ? 1 : 2;
        
        // if the next argument is not a flag(number or string) then set the map value to the next argument
        // if the next argument is a flag or the end of argument, then set the map value to empty string    
        std::string key = std::string(argv[argIndex]).substr(substrLen);
        parameters[key] = std::string("");
        ++argIndex;
        if((argIndex < argc) && (argv[argIndex][0] != '-')){
            parameters[key] = std::string(argv[argIndex]);
            ++argIndex;
        }
    }
}

bool argumentParser::has(const std::string& key){
    return this->parameters.find(key) != this->parameters.end();
}

const std::string argumentParser::get(const std::string& key){
    return this->parameters[key];
}