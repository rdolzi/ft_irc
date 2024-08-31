#include "channel.hpp"
#include <algorithm>


Channel::Channel() : _inviteOnly(false), _userLimit(-1) {}


Channel::Channel(const std::string& name) : _name(name), _inviteOnly(false), _topicRestricted(false), _userLimit(-1) {}


Channel::Channel(const Channel& other)
    : _name(other._name), _topic(other._topic), _members(other._members),
      _operators(other._operators), _key(other._key), _inviteOnly(other._inviteOnly),
      _topicRestricted(other._topicRestricted),
      _userLimit(other._userLimit) {}

Channel::~Channel() {}

Channel& Channel::operator=(const Channel& other) {
    if (this != &other) {
        _name = other._name;
        _topic = other._topic;
        _members = other._members;
        _operators = other._operators;
        _key = other._key;
        _inviteOnly = other._inviteOnly;
        _topicRestricted = other._topicRestricted;
        _userLimit = other._userLimit;
    }
    return *this;
}

std::string Channel::getName() const { return _name; }
std::string Channel::getTopic() const { return _topic; }
std::vector<Client*> Channel::getMembers() const { return _members; }
std::vector<Client*> Channel::getOperators() const { return _operators; }
std::string Channel::getKey() const { return _key; }
bool Channel::isInviteOnly() const { return _inviteOnly; }

void Channel::setTopic(const std::string& topic) { _topic = topic; }
void Channel::setKey(const std::string& key) { _key = key; }
void Channel::setInviteOnly(bool inviteOnly) { _inviteOnly = inviteOnly; }
void Channel::setUserLimit(int limit) { _userLimit = limit; }

void Channel::removeMember(Client* client) {
    _members.erase(std::remove(_members.begin(), _members.end(), client), _members.end());
    removeOperator(client);
}

void Channel::addOperator(Client* client) {
    _operators.push_back(client);
}

void Channel::removeOperator(Client* client) {
    _operators.erase(std::remove(_operators.begin(), _operators.end(), client), _operators.end());
}

bool Channel::isMember(Client* client) const {
    return std::find(_members.begin(), _members.end(), client) != _members.end();
}

bool Channel::isOperator(Client* client) const {
    return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
}

bool Channel::isInvited(Client* client) const {
    return std::find(_invitedClients.begin(), _invitedClients.end(), client) != _invitedClients.end();
}

bool Channel::checkKey(const std::string& key) const {
    return _key.empty() || key == _key;
}

bool Channel::isFull() const {
    return _userLimit > 0 && _members.size() >= static_cast<size_t>(_userLimit);
}

std::string Channel::getNames() const {
    std::string names;
    for (std::vector<Client*>::const_iterator it = _members.begin(); it != _members.end(); ++it) {
        if (!names.empty()) {
            names += " ";
        }
        names += (*it)->getNickname();
    }
    return names;
}

void Channel::inviteClient(Client* client) {
    if (!isInvited(client)) {
        _invitedClients.push_back(client);
    }
}

// Update addMember to remove the client from the invited list if they join
bool Channel::addMember(Client* client, const std::string& key) {
    if (isFull() || !checkKey(key)) {
        return false;
    }
    
    if (_inviteOnly && !isInvited(client)) {
        return false;
    }
    
    if (!isMember(client)) {
        _members.push_back(client);
        _invitedClients.erase(std::remove(_invitedClients.begin(), _invitedClients.end(), client), _invitedClients.end());
        return true;
    }
    
    return false;
}




void Channel::setTopicRestricted(bool restricted) {
    _topicRestricted = restricted;
}

void Channel::removeKey() {
    _key.clear();
}



void Channel::removeUserLimit() {
    _userLimit = -1;
}



bool Channel::isTopicRestricted() const {
    return _topicRestricted;
}

