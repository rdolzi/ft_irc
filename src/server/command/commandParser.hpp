#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP

#include "command.hpp"
#include <string>

class CommandParser {
public:
    static Command parse(const std::string& message);

private:
    static std::string extractPrefix(std::string& message);
    static std::string extractCommand(std::string& message);
    static std::vector<std::string> extractParameters(std::string& message);
};

#endif // COMMANDPARSER_HPP