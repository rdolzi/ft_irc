#include "command.hpp"
#include <sstream>

Command::Command() {}

Command::Command(const std::string& prefix, const std::string& command, const std::vector<std::string>& parameters)
    : _prefix(prefix), _command(command), _parameters(parameters) {}

std::string Command::getPrefix() const {
    return _prefix;
}

std::string Command::getCommand() const {
    return _command;
}

std::vector<std::string> Command::getParameters() const {
    return _parameters;
}

void Command::setPrefix(const std::string& prefix) {
    _prefix = prefix;
}

void Command::setCommand(const std::string& command) {
    _command = command;
}

void Command::addParameter(const std::string& parameter) {
    _parameters.push_back(parameter);
}

std::string Command::toString() const {
    std::stringstream ss;
    if (!_prefix.empty()) {
        ss << ":" << _prefix << " ";
    }
    ss << _command;
    for (size_t i = 0; i < _parameters.size(); ++i) {
        ss << " " << _parameters[i];
    }
    return ss.str();
}

bool Command::isValid() const {
    return !_command.empty();
}