#include "commandExecutor.hpp"


CommandExecutor::CommandExecutor(Server& server) : _server(server) {}

CommandExecutor::~CommandExecutor() {}

void CommandExecutor::executeCommand(int clientFd, const Command& cmd) {
    std::string command = cmd.getCommand();
    Client* client = _server.getClientByFd(clientFd);

    if (!client) {
        Logger::error("Client not found for fd: " + std::to_string(clientFd));
        return;
    }

    if (!isRegistered(client) && command != "PASS" && command != "NICK" && command != "USER") {
        sendReply(clientFd, "451 * :You have not registered");
        return;
    }

    if (command == "PASS") {
        executePass(clientFd, cmd);
    } else if (command == "NICK") {
        executeNick(clientFd, cmd);
    } else if (command == "USER") {
        executeUser(clientFd, cmd);
    } else if (command == "JOIN") {
        executeJoin(clientFd, cmd);
    } else if (command == "PRIVMSG") {
        executePrivmsg(clientFd, cmd);
    }  else {
        Logger::warning("Unimplemented command: " + command);
        sendReply(clientFd, "421 * " + command + " :Unknown command");
    }
}

void CommandExecutor::executePass(int clientFd, const Command& cmd) {
    Client* client = _server.getClientByFd(clientFd);
    
    Logger::debug("Executing PASS command. Client password set: " + 
                  std::string(client->isPasswordSet() ? "true" : "false"));

    if (client->isPasswordSet()) {
        sendReply(clientFd, "462 * :You may not reregister");
        Logger::debug("Sent 'already registered' reply");
        return;
    }

    if (cmd.getParameters().empty()) {
        sendReply(clientFd, "461 PASS :Not enough parameters");
        Logger::debug("Sent 'not enough parameters' reply");
        return;
    }

    std::string password = cmd.getParameters()[0];
    if (password == _server.getPassword()) {
        client->setPassword(true);
        sendReply(clientFd, "001 * :Password accepted");
        Logger::debug("Sent 'password accepted' reply");
    } else {
        sendReply(clientFd, "464 * :Password incorrect");
        Logger::debug("Sent 'password incorrect' reply");
    }
}


void CommandExecutor::executeNick(int clientFd, const Command& cmd) {
    Client* client = _server.getClientByFd(clientFd);
    Logger::debug("Executing NICK command. Current nickname: " + client->getNickname());

    if (cmd.getParameters().empty()) {
        sendReply(clientFd, "431 :No nickname given");
        Logger::debug("Sent 'no nickname given' reply");
        return;
    }

    std::string newNick = cmd.getParameters()[0];

    if (!isValidNickname(newNick)) {
        sendReply(clientFd, "432 * " + newNick + " :Erroneous nickname");
        Logger::debug("Sent 'erroneous nickname' reply");
        return;
    }

    if (_server.isNicknameTaken(newNick)) {
        sendReply(clientFd, "433 * " + newNick + " :Nickname is already in use");
        Logger::debug("Sent 'nickname in use' reply");
        return;
    }

    std::string oldNick = client->getNickname();
    client->setNickname(newNick);

    if (oldNick.empty()) {
        Logger::debug("Nickname set for the first time: " + newNick);
    } else {
        _server.broadcast(":" + oldNick + " NICK " + newNick + "\r\n", clientFd);
        Logger::debug("Nickname changed from " + oldNick + " to " + newNick);
    }

   
    if (isRegistered(client)) {
        sendReply(clientFd, "001 " + newNick + " :Welcome to the Internet Relay Network " + newNick);
        Logger::debug("Registration complete, sent welcome message");
    }
}


void CommandExecutor::executeUser(int clientFd, const Command& cmd) {
    Client* client = _server.getClientByFd(clientFd);
    Logger::debug("Executing USER command. Is user set: " + std::string(client->isUserSet() ? "true" : "false"));

    if (client->isUserSet()) {
        sendReply(clientFd, "462 :You may not reregister");
        Logger::debug("Sent 'already registered' reply");
        return;
    }

    if (cmd.getParameters().size() < 4) {
        sendReply(clientFd, "461 USER :Not enough parameters");
        Logger::debug("Sent 'not enough parameters' reply");
        return;
    }

    std::string username = cmd.getParameters()[0];
    std::string realname = cmd.getParameters()[3];

    client->setUsername(username);
    client->setRealname(realname);
    client->setUser(true);

    Logger::debug("User info set - Username: " + username + ", Realname: " + realname);

    if (isRegistered(client)) {
        sendReply(clientFd, "001 " + client->getNickname() + " :Welcome to the Internet Relay Network " 
                  + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname());
        Logger::debug("Registration complete, sent welcome message");
    }
}

void CommandExecutor::executeJoin(int clientFd, const Command& cmd) {
    if (cmd.getParameters().empty()) {
        sendReply(clientFd, "461 JOIN :Not enough parameters");
        return;
    }

    std::string channelName = cmd.getParameters()[0];
    std::string key = (cmd.getParameters().size() > 1) ? cmd.getParameters()[1] : "";
    Client* client = _server.getClientByFd(clientFd);

    // TODO: Implement full channel joining logic, including:
    // - Channel creation if it doesn't exist
    // - Checking channel user limits
    // - Handling invite-only channels
    // - Handling channel keys
    // For now, we'll just send a success message

    _server.broadcast(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " JOIN " + channelName + "\r\n");
    // TODO: Send channel topic (332) and names list (353, 366)
}

void CommandExecutor::executePrivmsg(int clientFd, const Command& cmd) {
    if (cmd.getParameters().size() < 2) {
        sendReply(clientFd, "411 :No recipient given (PRIVMSG)");
        return;
    }

    std::string target = cmd.getParameters()[0];
    std::string message = cmd.getParameters()[1];
    Client* sender = _server.getClientByFd(clientFd);

    if (target[0] == '#') {
        // Channel message
        // TODO: Implement channel messaging
        // Check if channel exists, if user is in channel, if user can speak in channel
    } else {
        // Private message
        Client* recipient = _server.getClientByNickname(target);
        if (recipient) {
            sendReply(recipient->getFd(), ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname() + " PRIVMSG " + target + " :" + message);
        } else {
            sendReply(clientFd, "401 " + sender->getNickname() + " " + target + " :No such nick/channel");
        }
    }
}


bool CommandExecutor::isValidNickname(const std::string& nickname) const {
    if (nickname.empty() || nickname.length() > 9) {
        return false;
    }
    
    if (!std::isalpha(nickname[0]) && !strchr("[]\\`_^{|}", nickname[0])) {
        return false;
    }

    for (size_t i = 1; i < nickname.length(); ++i) {
        char c = nickname[i];
        if (!std::isalnum(c) && !strchr("-[]\\`^{|}", c)) {
            return false;
        }
    }

    return true;
}

bool CommandExecutor::isRegistered(const Client* client) const {
    return client->isPasswordSet() && !client->getNickname().empty() && client->isUserSet();
}


// void CommandExecutor::sendReply(int clientFd, const std::string& reply) const {
//     Client* client = _server.getClientByFd(clientFd);
//     std::string target = client->getNickname();
//     if (target.empty()) {
//         target = "*";  // Use * if nickname is not set yet
//     }
//     std::string formattedReply = ":" + _server.getServerName() + " " + reply;
//     _server.sendToClient(clientFd, formattedReply + "\r\n");
// }

void CommandExecutor::sendReply(int clientFd, const std::string& reply) const {
    std::string formattedReply = ":" + _server.getServerName() + " " + reply + "\r\n";
    Logger::debug("Sending reply to client " + std::to_string(clientFd) + ": " + formattedReply);
    _server.sendToClient(clientFd, formattedReply);
}