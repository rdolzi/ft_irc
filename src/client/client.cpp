#include "client.hpp"
#include <algorithm>

Client::Client(int fd) : _fd(fd), _isOperator(false), _isPasswordSet(false), _isUserSet(false), _passReceived(false){}

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


bool Client::isInChannel(const std::string& channel) const {
    return std::find(_channels.begin(), _channels.end(), channel) != _channels.end();
}

bool Client::isPasswordSet() const {
    return _isPasswordSet;
}

bool Client::isUserSet() const {
    return _isUserSet;
}

void Client::setRealname(const std::string& realname) {
    _realname = realname;
}

void Client::setHostname(const std::string& hostname) {
    _hostname = hostname;
}

std::string Client::getRealname() const {
    return _realname;
}

std::string Client::getHostname() const {
    return _hostname;
}

void Client::setPassword(bool isSet) {
    _isPasswordSet = isSet;
}

void Client::setUser(bool isSet) {
    _isUserSet = isSet;
}

std::string Client::getFullClientIdentifier() const {
    return _nickname + "!" + _username + "@" + _hostname;
}

bool Client::isPassReceived() const {
    return _passReceived;
}

void Client::setPassReceived(bool received) { _passReceived = received; }


// void Client::setRestricted(bool restricted) { _restricted = restricted; }
// bool Client::isRestricted() const { return _restricted; }



void Client::setInvisible(bool invisible) {
    if (invisible) {
        _modes.insert('i');
    } else {
        _modes.erase('i');
    }
}


void Client::setRestricted(bool restricted) {
    if (restricted) {
        _modes.insert('r');
    } else {
        _modes.erase('r');
    }
}

bool Client::isInvisible() const {
    return _modes.find('i') != _modes.end();
}

bool Client::receivesWallops() const {
    return _modes.find('w') != _modes.end();
}

bool Client::isRestricted() const {
    return _modes.find('r') != _modes.end();
}

void Client::setOperator(bool op) {
    _isOperator = op;
    if (op) {
        _modes.insert('o');
    } else {
        _modes.erase('o');
    }
}

std::string Client::getModeString() const {
    std::string modeString;
    for (std::set<char>::const_iterator it = _modes.begin(); it != _modes.end(); ++it) {
        modeString += *it;
    }
    return modeString;
}