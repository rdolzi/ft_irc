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

    // Helper methods
    bool isValidNickname(const std::string& nickname) const;
    bool isRegistered(const Client* client) const;
    void sendReply(int clientFd, const std::string& reply) const;

public:
    CommandExecutor(Server& server);
    ~CommandExecutor();

    void executeCommand(int clientFd, const Command& cmd);

private:
    void executePass(int clientFd, const Command& cmd);
    void executeNick(int clientFd, const Command& cmd);
    void executeUser(int clientFd, const Command& cmd);
    void executeJoin(int clientFd, const Command& cmd);
    void executePrivmsg(int clientFd, const Command& cmd);
    // more command execution methods here ...
};

#endif // COMMANDEXECUTOR_HPP