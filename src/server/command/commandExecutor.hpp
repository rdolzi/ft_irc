/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commandExecutor.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 20:40:00 by rdolzi            #+#    #+#             */
/*   Updated: 2024/09/07 20:40:03 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDEXECUTOR_HPP
#define COMMANDEXECUTOR_HPP

#include "../server.hpp"
#include "../../client/client.hpp"
#include "../../logger/logger.hpp"
#include <algorithm>
#include "command.hpp"
#include <string>
#include <vector>

class Server;

class CommandExecutor {
private:
    Server& _server;

    void executePass(int clientFd, const Command& cmd);
    void executeNick(int clientFd, const Command& cmd);
    void executeUser(int clientFd, const Command& cmd);
    void executeJoin(int clientFd, const Command& cmd);
    void executePrivmsg(int clientFd, const Command& cmd);
    void executeMode(int clientFd, const Command& cmd);
    void executeTopic(int clientFd, const Command& cmd);
    void executeInvite(int clientFd, const Command& cmd);
    void executeKick(int clientFd, const Command& cmd);
    void executePing(int clientFd, const Command& cmd);
    void executeCap(int clientFd);
    void executeWho(int clientFd, const Command& cmd);
    void executeNotice(int clientFd, const Command& cmd);

    // Helper methods
    bool isValidNickname(const std::string& nickname) const;
    bool isRegistered(const Client* client) const;
    void sendReply(int clientFd, const std::string& reply, bool flag) const;
    void handleChannelMode(int clientFd, const std::string& channelName, const std::string& modestring, const std::vector<std::string>& args);
     bool isValidChannelName(const std::string& channelName);

public:
    CommandExecutor(Server& server);
    ~CommandExecutor();

    void executeCommand(int clientFd, const Command& cmd);
    bool isChannelSyntaxOk(const std::string& channelName);
    
};

#endif // COMMANDEXECUTOR_HPP