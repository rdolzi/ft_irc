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

    // Helper methods
    bool isValidNickname(const std::string& nickname) const;
    bool isRegistered(const Client* client) const;
    void sendReply(int clientFd, const std::string& reply) const;
    bool isChannelNameValid(const std::string& channelName) const;
    bool canUserSpeakInChannel(const Client* client, const std::string& channelName) const;
    void handleChannelMode(int clientFd, const std::string& channelName, const std::string& modestring, const std::vector<std::string>& args);
    void handleUserMode(int clientFd, const std::string& nickname, const std::string& modestring);

public:
    CommandExecutor(Server& server);
    ~CommandExecutor();

    void executeCommand(int clientFd, const Command& cmd);
    
};

#endif // COMMANDEXECUTOR_HPP