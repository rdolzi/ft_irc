#include "client.hpp"
#include <algorithm>

Client::Client(int fd) : _fd(fd), _isOperator(false) {}

Client::~Client() {}

int Client::getFd() const {
    return _fd;
}

std::string Client::getNickname() const {
    return _nickname;
}

std::string Client::getUsername() const {
    return _username;
}

std::vector<std::string> Client::getChannels() const {
    return _channels;
}

bool Client::isOperator() const {
    return _isOperator;
}

void Client::setNickname(const std::string& nickname) {
    _nickname = nickname;
}

void Client::setUsername(const std::string& username) {
    _username = username;
}

void Client::addChannel(const std::string& channel) {
    if (!isInChannel(channel)) {
        _channels.push_back(channel);
    }
}

void Client::removeChannel(const std::string& channel) {
    _channels.erase(std::remove(_channels.begin(), _channels.end(), channel), _channels.end());
}

void Client::setOperator(bool isOperator) {
    _isOperator = isOperator;
}

bool Client::isInChannel(const std::string& channel) const {
    return std::find(_channels.begin(), _channels.end(), channel) != _channels.end();
}