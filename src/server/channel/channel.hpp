/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 20:40:20 by rdolzi            #+#    #+#             */
/*   Updated: 2024/09/07 20:40:20 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "../../client/client.hpp"
#include "../../logger/logger.hpp"

class Channel {
private:
    std::string _name;
    std::string _topic;
    std::vector<Client*> _members;
    std::vector<Client*> _operators;
    std::string _key;
    std::vector<Client*> _invitedClients;
    bool _inviteOnly;
    bool _topicRestricted;
    int _userLimit;

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


    // Setters
    void setTopic(const std::string& topic);
    void setKey(const std::string& key);
    void setInviteOnly(bool inviteOnly);
    void setUserLimit(int limit);

    // Member functions
    int addMember(Client* client, const std::string& key);
    void removeMember(Client* client);
    void addOperator(Client* client);
    void removeOperator(Client* client);
    bool isMember(Client* client) const;
    bool isOperator(Client* client) const;
    bool isInvited(Client* client) const;
    bool checkKey(const std::string& key) const;
    bool isFull() const;
    std::string getNames() const;
    void inviteClient(Client* client);
    void setTopicRestricted(bool restricted);
    void removeKey();
    void removeUserLimit();
    bool isTopicRestricted() const;
};

#endif // CHANNEL_HPP