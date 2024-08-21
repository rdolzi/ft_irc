#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "../../client/client.hpp"

class Channel {
private:
    int _userLimit;
    bool _inviteOnly;
    std::string _name;
    std::string _topic;
    std::string _key;
    std::vector<Client*> _members;
    std::vector<Client*> _operators;
    std::vector<Client*> _invitedClients;

public:
    Channel();
    Channel(const std::string& name);
    Channel(const Channel& other);
    ~Channel();
    Channel& operator=(const Channel& other);

    // Getters
    std::string getName() const;
    std::string getTopic() const;
    std::vector<Client*> getMembers() const;
    std::vector<Client*> getOperators() const;
    std::string getKey() const;
    bool isInviteOnly() const;
    int getUserLimit() const;

    // Setters
    void setTopic(const std::string& topic);
    void setKey(const std::string& key);
    void setInviteOnly(bool inviteOnly);
    void setUserLimit(int limit);

    // Member functions
    bool addMember(Client* client, const std::string& key);
    void removeMember(Client* client);
    void addOperator(Client* client);
    void removeOperator(Client* client);
    bool isMember(Client* client) const;
    bool isOperator(Client* client) const;
    size_t getMemberCount() const;
    bool isInvited(Client* client) const;
    bool checkKey(const std::string& key) const;
    bool isFull() const;
    std::string getNames() const;
    void inviteClient(Client* client);
};

#endif // CHANNEL_HPP