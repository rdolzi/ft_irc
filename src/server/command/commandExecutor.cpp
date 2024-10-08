/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commandExecutor.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 20:40:05 by rdolzi            #+#    #+#             */
/*   Updated: 2024/09/07 20:40:10 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commandExecutor.hpp"


CommandExecutor::CommandExecutor(Server& server) : _server(server) {}

CommandExecutor::~CommandExecutor() {}


void CommandExecutor::executeCommand(int clientFd, const Command& cmd) {
    std::string command = cmd.getCommand();
    Client* client = _server.getClientByFd(clientFd);

    if (!client) {
        Logger::error("Client not found for fd: " + to_string(clientFd));
        return;
    }

    // Check for too many parameters
    if (cmd.getParameters().size() > 15) {
        sendReply(clientFd, command + " :Too many parameters. Max is set to 15.", true);
        return;
    }


    if (command == "PING") {
        executePing(clientFd, cmd);
        return;
    }

    if (command == "CAP") {
        executeCap(clientFd);
        return;
    }

    if (command == "PASS") {
        executePass(clientFd, cmd);
        return;
    }

    // Check if PASS has been received before allowing other commands
    if (!client->isPasswordSet()) {
        sendReply(clientFd, " * :Password required", true);
        return;
    }

    if (!isRegistered(client) && command != "PASS" && command != "NICK" && command != "USER") {
        sendReply(clientFd, "451 * :You have not registered", true);
        return;
    }
    

    if (command == "NICK") {
       executeNick(clientFd, cmd);
    } else if (command == "USER") {
        executeUser(clientFd, cmd);
    } else if (command == "NOTICE") {
        executeNotice(clientFd, cmd);
    } else if (command == "JOIN") {
        executeJoin(clientFd, cmd);
    } else if (command == "PRIVMSG") {
        executePrivmsg(clientFd, cmd);
    }  else if (command == "MODE") {
        executeMode(clientFd, cmd);
    } else if (command == "TOPIC") {
        executeTopic(clientFd, cmd);
    } else if (command == "WHO") {
        executeWho(clientFd, cmd);
    } else if (command == "INVITE") {
        executeInvite(clientFd, cmd);
    } else if (command == "KICK") {
        executeKick(clientFd, cmd);
    }else {
        Logger::warning("Unimplemented command: " + command);
        sendReply(clientFd, "421 * " + command + " :Unknown command", true);
    }
}


void CommandExecutor::executePass(int clientFd, const Command& cmd) {
    Client* client = _server.getClientByFd(clientFd);
    
    Logger::debug("Executing PASS command. Client password set: " + 
                  std::string(client->isPasswordSet() ? "true" : "false"));

    if (client->isPasswordSet()) {
        sendReply(clientFd, "462 * :Unauthorized command (already registered)", true);
        Logger::debug("Sent [462] 'already registered' reply");
        return;
    }

    if (cmd.getParameters().empty()) {
        sendReply(clientFd, "461 PASS :Wrong number of parameters", true);
        Logger::debug("Sent [461] 'Wrong number of parameters' reply");
        return;
    }

    std::string password = cmd.getParameters()[0];
    if (password[0] == ':')
        password = password.substr(1);
    if (password == _server.getPassword()) {
        client->setPassword(true);
        //sendReply(clientFd, ":Password accepted");
        Logger::debug("Client "+ to_string(clientFd)  + " set password correctly.");
    } else {
        sendReply(clientFd, "464 * :Password incorrect", true);
        Logger::debug("Sent [464] 'password incorrect' reply");
    }
}


void CommandExecutor::executeNick(int clientFd, const Command& cmd) {
    Client* client = _server.getClientByFd(clientFd);

    // Ensure client is valid
    if (!client) {
        Logger::error("Client not found for fd: " + to_string(clientFd));
        return;
    }

    Logger::debug("Executing NICK command. Current nickname: " + client->getNickname());

    // Check if a nickname was provided
    if (cmd.getParameters().size() != 1) {
        sendReply(clientFd, "431 * :No nickname given", true);
        Logger::debug("Sent [431] 'no nickname given' reply");
        return;
    }

    std::string newNick = cmd.getParameters()[0];

    // Check if the provided nickname is valid
    if (!isValidNickname(newNick)) {
        sendReply(clientFd, "432 " + newNick + " :Erroneous nickname", true);
        Logger::debug("Sent [432] 'erroneous nickname' reply");
        return;
    }

    // Check if the nickname is already taken
    if (_server.isNicknameTaken(newNick)) {
        sendReply(clientFd, "433 " + newNick + " :Nickname is already in use", true);
        Logger::debug("Sent [433] 'nickname in use' reply");
        return;
    }

    std::string oldNick = client->getNickname();
    std::string oldClientIdentifier = client->getFullClientIdentifier();
    client->setNickname(newNick);

    // Handle first-time nickname set (registration complete)
    if (oldNick.empty() && isRegistered(client)) {
        Logger::debug("Nickname set for the first time: " + newNick);
        sendReply(clientFd, "001 " + newNick + " :Welcome to the Internet Relay Network " + client->getFullClientIdentifier(), true);
        Logger::debug("[001] Registration complete, sent welcome message");
    } else if (!oldNick.empty()) { 
        // Broadcast the nickname change to others
        std::string newClientIdentifier = client->getFullClientIdentifier();
        _server.broadcast(":" + oldClientIdentifier + " NICK " + newNick + "\r\n", clientFd);
        Logger::debug("Nickname changed from " + oldNick + " to " + newNick + ". Broadcasting to other clients.");
    } else {
        Logger::debug("Nickname set but client is not yet fully registered: " + newNick);
    }
}



void CommandExecutor::executeUser(int clientFd, const Command& cmd) {
    Client* client = _server.getClientByFd(clientFd);
    Logger::debug("Executing USER command. Is user set: " + std::string(client->isUserSet() ? "true" : "false"));

    if (client->isUserSet()) {
        sendReply(clientFd, "462 :Unauthorized command (already registered)", true);
        Logger::debug("Sent [462] 'already registered' reply");
        return;
    }

    if (cmd.getParameters().size() < 4) {
        sendReply(clientFd, "461 USER :Wrong number of parameters", true);
        Logger::debug("Sent [461] 'Wrong number of parameters' reply");
        return;
    }


    if (cmd.getParameters().size() > 4 || cmd.getParameters()[3][0] != ':') {
        sendReply(clientFd, "461 USER :Wrong syntax", true);
        Logger::debug("Sent [461] 'wrong syntax' reply");
        return;
    }

    std::string username = cmd.getParameters()[0];
    std::string realname = (cmd.getParameters()[3][0] == ':' ? cmd.getParameters()[3].substr(1) : cmd.getParameters()[3]);

    client->setUsername(username);
    client->setRealname(realname);
    client->setUser(true);

    Logger::debug("User info set - Username: " + username + ", Realname: " + realname);

    if (isRegistered(client)) {
        sendReply(clientFd, "001 " + client->getNickname() + " :Welcome to the Internet Relay Network " + client->getFullClientIdentifier(), true);
        Logger::debug("Registration complete, sent welcome message");
    }
}

void CommandExecutor::executeJoin(int clientFd, const Command& cmd) {
    if (cmd.getParameters().empty()) {
        sendReply(clientFd, "461 JOIN :Wrong number of parameters", true);
        return;
    }

    std::string channelName = cmd.getParameters()[0];
    std::string key;
    if (cmd.getParameters().size() > 1) {
    // Start concatenating the parameters from the second one
        for (size_t i = 1; i < cmd.getParameters().size(); ++i) {
            if (i > 1) {
                key += " "; // Add a space between parts if there are multiple
            }
            key += cmd.getParameters()[i];
    }

    // Check if the key starts with a colon and remove it
    if (!key.empty() && key[0] == ':') {
        key = key.substr(1);
    }
}

    Client* client = _server.getClientByFd(clientFd);

    if (!client) {
        Logger::error("Client not found for fd: " + to_string(clientFd));
        return;
    }

    // Check if the channel name is valid
    if (!isValidChannelName(channelName)) {
        sendReply(clientFd, "403 " + channelName + " :No such channel", true);
        Logger::debug("Sent [403] 'No such channel' reply");
        return;
    }

    // Create or get the channel
    Channel* channel = _server.getOrCreateChannel(channelName, clientFd);
    if (!channel) {
        sendReply(clientFd, "403 " + channelName + " :No such channel", true);
        Logger::debug("Sent [403] 'No such channel' reply");
        return;
    }

    if (channel->getMembers().empty()) {
        channel->addOperator(client);
        Logger::info("Client " + client->getNickname() + " is now the channel operator for " + channelName);
    }

    int res = channel->addMember(client, key);
    // Add the client as a member of the channel
    if (res == 1) {
        sendReply(client->getFd(), " :Can't join, channel is full", true);
        Logger::error("Failed to add member to channel after all checks passed");
        return;
    } else if (res == 2) {
        sendReply(client->getFd(), " :Can't join, wrong channel key", true);
        Logger::error("Failed to add member to channel after all checks passed");
        return;
    } if (res == 3 ) {
        sendReply(clientFd, channelName + " :Can't join channel, channel is invite only", true);
        Logger::error("Failed to add member to channel after all checks passed");
        return;
    } if (res == 4 ) {
        sendReply(clientFd, channelName + " :Can't join channel", true);
        Logger::error("Failed to add member to channel after all checks passed");
        return;
    } 

    // Broadcast the join message to all members of the channel
    std::string joinMessage = ":" + client->getFullClientIdentifier() + " JOIN :" + channelName + "\r\n";
    _server.broadcastToChannel(channelName, joinMessage);

    // Send the channel topic after joining
    std::string topic = channel->getTopic();
    if (topic.empty()) {
        sendReply(clientFd, "331 " + client->getNickname() + " " + channelName + " :No topic is set", true);
    } else {
        sendReply(clientFd, "332 " + client->getNickname() + " " + channelName + " :" + topic, true);
    }

    // Send names list (numeric 353 and 366)
    std::string namesList = channel->getNames();
    sendReply(clientFd, "353 " + client->getNickname() + " = " + channelName + " :" + namesList, true);
    sendReply(clientFd, "366 " + client->getNickname() + " " + channelName + " :End of /NAMES list", true);
}

void CommandExecutor::executePrivmsg(int clientFd, const Command& cmd) {
    // Check if the PRIVMSG command has at least two parameters: the target and the message
    if (cmd.getParameters().size() < 2) {
        sendReply(clientFd, "411 :No recipient given (PRIVMSG)", true);
        return;
    }

    std::string target = cmd.getParameters()[0];
    std::string message = cmd.getParameters()[1];
    Client* sender = _server.getClientByFd(clientFd);

    // Check if the sender exists (shouldn't be null)
    if (!sender) {
        Logger::error("Client not found for fd: " + to_string(clientFd));
        return;
    }

    // Check if the message starts with ':'
    if (message[0] != ':') {
        sendReply(clientFd, "461 PRIVMSG :Message must start with ':'", true);  // Error code 461: Not enough parameters
        Logger::debug("PRIVMSG message missing ':'");
        return;
    }

    // Remove the leading ':' from the message
    message = message.substr(1);

    // Check if the target is a channel
    if (isChannelSyntaxOk(target)) {
        // Validate the channel name
        if (!isValidChannelName(target)) {
            sendReply(clientFd, "403 :No such channel", true);
            Logger::debug("Sent [403] 'No such channel' reply");
            return;
        }

        Channel* channel = _server.getChannel(target);

        // Check if the channel exists and the sender is a member of the channel
        if (!channel || !channel->isMember(sender)) {
            sendReply(clientFd, "404 " + target + " :Cannot send to channel", true);
            Logger::debug("Client is not a member of the channel. Sent [404] ':Cannot send to channel'");
            return;
        }

        Logger::info("Client is sending a message to the channel...");

        // Construct the channel message in the proper IRC format
        std::string channelMessage = ":" + sender->getFullClientIdentifier() + " PRIVMSG " + target + " :" + message + "\r\n";
        _server.broadcastToChannel(target, channelMessage, sender);

    } else {
        // Handle private messages to a specific user
        Client* recipient = _server.getClientByNickname(target);

        // Check if the recipient exists
        if (!recipient) {
            Logger::error("Recipient client not found for nickname: " + target);
            sendReply(clientFd, "401 " + target + " :No such nick/channel", true);
            return;
        }

        Logger::info("Target: " + target + " | Recipient: " + recipient->getNickname());

        // Construct the private message in the proper IRC format
        std::string privateMessage = ":" + sender->getFullClientIdentifier() + " PRIVMSG " + target + " :" + message + "\r\n";
        sendReply(recipient->getFd(), privateMessage, false);

        Logger::info("Message sent to recipient: " + recipient->getNickname());
    }
}

/*
It checks that the nickname is not empty and not longer than 9 characters.
The first character must be a letter or one of the special characters defined in the RFC.
The remaining characters can be letters, digits, hyphens, or the special characters.
*/
bool CommandExecutor::isValidNickname(const std::string& nickname) const {
    if (nickname.empty() || nickname.length() > 9) {
        return false;
    }

    // First character must be a letter or special character
    if (!std::isalpha(nickname[0]) && nickname.find_first_of("[]\\`_^{|}") != 0) {
        return false;
    }

    // Rest of the characters
    for (size_t i = 1; i < nickname.length(); ++i) {
        char c = nickname[i];
        if (!std::isalnum(c) && c != '-' && nickname.find_first_of("[]\\`_^{|}") != i) {
            return false;
        }
    }

    return true;
}

bool CommandExecutor::isRegistered(const Client* client) const {
    return client->isPasswordSet() && (client->getNickname() != "") && client->isUserSet();
}

void CommandExecutor::sendReply(int clientFd, const std::string& reply, bool includeServerName) const {
    std::string formattedReply = reply;
    
    if (includeServerName) {
        formattedReply = ":" + _server.getServerName() + " " + reply + "\r\n";  // Include server name if needed
    } else {
        formattedReply += "\r\n";  // Just append the reply without server name
    }
    
    Logger::debug("Sending reply to client " + to_string(clientFd) + ": " + formattedReply);
    _server.sendToClient(clientFd, formattedReply);  // Send reply only to the specified client
}



void CommandExecutor::executeMode(int clientFd, const Command& cmd) {
    if (cmd.getParameters().size() == 1) {
        sendReply(clientFd, "368 MODE "+cmd.getParameters()[0], true);
        return;
    }

    if (cmd.getParameters().size() == 2 &&  cmd.getParameters()[1] == "b") {
        sendReply(clientFd, "368 MODE "+cmd.getParameters()[0]+" :End of channel ban list", true);
        return;
    }

    if (cmd.getParameters().size() > 3) {
        sendReply(clientFd, "461 MODE :Wrong number of parameters", true);
        return;
    }
    std::string channel = cmd.getParameters()[0];
    std::string modestring = cmd.getParameters()[1];

    std::vector<std::string> args;
     for (size_t i = 0; i < cmd.getParameters().size(); ++i) {
        Logger::info("Parameter " + to_string(i) + ": '" + cmd.getParameters()[i] + "'");
        args.push_back(cmd.getParameters()[i]);
    }
    // Check if the channel name is valid
    if (!isValidChannelName(channel)) {
        sendReply(clientFd, "476 " + channel + " :Bad Channel Mask", true);
        return;
    }
    handleChannelMode(clientFd, channel, modestring, args);
}

void CommandExecutor::handleChannelMode(int clientFd, const std::string& channelName, const std::string& modestring, const std::vector<std::string>& args) {
    // 1. Validate the channel
    Channel* channel = _server.getChannel(channelName);
    if (!channel) {
        sendReply(clientFd, "403 " + channelName + " :No such channel", true);
        return;
    }
    // 2. Initialize variables for processing modes
    Client* client = _server.getClientByFd(clientFd);

    if (!client) {
        Logger::error("Client not found in handleChannelMode for fd: " + to_string(clientFd));
        return;
    }

    bool adding = true;
    size_t argIndex = 0;
    std::string modeChanges;
    std::string modeArgs;
    int paramModeCount = 0;

    if (!channel->isOperator(client)){
        sendReply(clientFd, "481 "+client->getFullClientIdentifier()+" :Permission Denied - You do not have the required privileges", true);
        return;
    }
    // 3. Process each character in the modestring
    for (size_t i = 0; i < modestring.length(); ++i) {
        char mode = modestring[i];
        if (mode == '+') {
            adding = true;
            modeChanges += "+";
            argIndex++;
        } else if (mode == '-') {
            adding = false;
            modeChanges += "-";
            argIndex++;
        } else {
            bool requiresParam = (mode == 'k' || mode == 'o' || mode == 'l');
            if (requiresParam && paramModeCount >= 3) {
                break;  // Max 3 parameter modes per command
            }

            switch (mode) {
                case 'i':
                    channel->setInviteOnly(adding);
                    modeChanges += mode;
                    argIndex++;
                    break;
                case 't':
                    channel->setTopicRestricted(adding);
                    modeChanges += mode;
                    argIndex++;
                    break;
                case 'k': // Channel key (password)
                    if (adding && argIndex < args.size()) {
                        channel->setKey(args[argIndex + 1]);
                        modeChanges += mode;
                        modeArgs += " " + args[argIndex];
                        argIndex++;
                        paramModeCount++;
                    } else if (!adding) {
                        channel->removeKey();
                        modeChanges += mode;
                        paramModeCount++;
                        argIndex++;
                    }
                    break;
                case 'o':
                    if (args.size() == 3) {
                        Client* targetClient = _server.getClientByNickname(args[2]);
                        if (!targetClient) {
                            Logger::error("Client not found in handleChannelMode for fd: " + to_string(clientFd));
                            return;
                        }
                        Logger::info("targetClient: "+ targetClient->getNickname());
                        if (targetClient) {
                            if (adding) {
                                channel->addOperator(targetClient);
                            } else {
                                channel->removeOperator(targetClient);
                            }
                            modeChanges += mode;
                           // modeArgs += " " + args[argIndex];
                            paramModeCount++;
                        }
                        argIndex++;
                    }
                    break;
                case 'l': // User limit
                Logger::info("In mode (l)");
                    if (adding && argIndex < args.size()) {
                        Logger::info("Inside if mode (l)");
                        int userLimit;
                        std::stringstream ss(args[argIndex + 1]);
                        ss >> userLimit;

                        if (ss.fail() || !ss.eof()) {
                            Logger::error("Error while executing MODE (l)");
                        } else {
                            Logger::info("In mode (l)");
                            channel->setUserLimit(userLimit);
                            modeChanges += mode;
                            modeArgs += " " + args[argIndex];
                            paramModeCount++;
                        }
                    }
                    else if (!adding) {
                        channel->removeUserLimit();
                        modeChanges += mode;
                    }
                    argIndex++;
                    break;
                default:
                    sendReply(clientFd, "501 " + std::string(1, mode) + " :is unknown mode char to me", true);
                    argIndex++;
                    break;
            }
        }
    }

    // 4. Build the mode change string && broadcast
    if (!modeChanges.empty()) {
        std::string modeMessage = ":" + client->getNickname() + " MODE " + channelName + " " + modeChanges + modeArgs + "\r\n";
        _server.broadcastToChannel(channelName, modeMessage);
    }
}


void CommandExecutor::executeTopic(int clientFd, const Command& cmd) {
    Client* client = _server.getClientByFd(clientFd);

    if (cmd.getParameters().empty() || cmd.getParameters().size() > 2) {
        Logger::debug("Invalid number of parameters for TOPIC command");
        sendReply(clientFd, "461 TOPIC :Wrong number of parameters", true);
        return;
    }

    std::string channelName = cmd.getParameters()[0];
    Channel* channel = _server.getChannel(channelName);
    if (!channel) {
        Logger::debug("Channel " + channelName + " does not exist");
        sendReply(clientFd, "403 " + channelName + " :No such channel", true);
        return;
    }

    if (!channel->isMember(client)) {
        Logger::debug("Client " + client->getNickname() + " is not a member of channel " + channelName);
        sendReply(clientFd, "442 " + channelName + " :You're not on that channel", true);
        return;
    }


    if (!channel->isOperator(client)) {
        sendReply(clientFd, "482 " + channelName + " :You're not channel operator", true);
        return;
    }
    
    if (channel->isTopicRestricted()) {
        sendReply(clientFd, " " + channelName + " :topic is restricted (+t)", false);
        return;
    }

    // Querying the topic
    if (cmd.getParameters().size() == 1) {
        Logger::debug("Querying the topic for channel " + channelName);
        std::string currentTopic = channel->getTopic();
        if (currentTopic.empty()) {
            Logger::debug("No topic is set for channel " + channelName);
            sendReply(clientFd, "331 " + client->getNickname() + " " + channelName + " :No topic is set", true);
        } else {
            Logger::debug("Sending topic for channel " + channelName);
            sendReply(clientFd, "332 " + client->getNickname() + " " + channelName + " :" + currentTopic, false); // No server name
        }
        return;
    }

    // Setting the topic
    if (!channel->isOperator(client)) {
        Logger::debug("Client " + client->getNickname() + " is not an operator in channel " + channelName);
        sendReply(clientFd, "482 " + channelName + " :You're not channel operator", true);
        return;
    }

    std::string newTopic = cmd.getParameters()[1];

    // Check if the new topic starts with a colon (IRC requirement for multi-word topics)
    if (newTopic.empty() || newTopic[0] != ':') {
        Logger::debug("Invalid topic format for channel " + channelName);
        sendReply(clientFd, "461 TOPIC :Topic must start with ':'", true);
        return;
    }

    // Remove the leading colon for storage
    newTopic = newTopic.substr(1);

    Logger::debug("Setting new topic for channel " + channelName + ": " + newTopic);
    channel->setTopic(newTopic);

    // Broadcast the new topic to all members of the channel
    std::string topicMessage = ":" + client->getFullClientIdentifier() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
    _server.broadcastToChannel(channelName, topicMessage);
}


void CommandExecutor::executeInvite(int clientFd, const Command& cmd) {
    Client* inviter = _server.getClientByFd(clientFd);


    // Check for correct number of parameters
    if (cmd.getParameters().size() != 2) {
        sendReply(clientFd, "461 INVITE :Wrong number of parameters", true);
        return;
    }

    std::string inviterNick = cmd.getParameters()[0];
    std::string channelName = cmd.getParameters()[1];

    // Check if the invitee exists
    Client* invitee = _server.getClientByNickname(inviterNick);
    if (!invitee) {
        sendReply(clientFd, "401 " + inviterNick + " :No such nick/channel", true);
        return;
    }

    // Check if the channel exists
    Channel* channel = _server.getChannel(channelName);
    if (!channel) {
        sendReply(clientFd, "403 " + channelName + " :No such channel", true);
        return;
    }
    
    // Check if the inviter is on the channel
    if (!channel->isMember(inviter)) {
        sendReply(clientFd, "442 " + channelName + " :You're not on that channel", true);
        return;
    }

    if (!channel->isOperator(inviter)){
    sendReply(clientFd, "482 "+inviter->getFullClientIdentifier()+" "+channelName+ " :You're not channel operator", true);
    return;
    }

    // Check if the invitee is already on the channel
    if (channel->isMember(invitee)) {
        sendReply(clientFd, "443 " + inviterNick + " " + channelName + " :is already on channel", true);
        return;
    }

    // Check if the channel is invite-only and if the inviter has the necessary privileges
    if (!channel->isOperator(inviter)) {
        sendReply(clientFd, "482 "+inviter->getFullClientIdentifier()+" "+channelName+ " :You're not channel operator", true);
        return;
    }

    // All checks passed, proceed with the invitation
    channel->inviteClient(invitee);

    // Send confirmation to the inviter
    sendReply(clientFd, "341 " + inviterNick + " " + channelName, true);

    // Send invitation to the invitee
    std::string inviteMsg = ":" + inviter->getFullClientIdentifier() + " INVITE " + inviterNick + " :" + channelName + "\r\n";
    _server.sendToClient(invitee->getFd(), inviteMsg);

}


void CommandExecutor::executeKick(int clientFd, const Command& cmd) {
    Client* kicker = _server.getClientByFd(clientFd);

    // Check for correct number of parameters
    if (cmd.getParameters().size() < 2 || cmd.getParameters().size() > 3) {
        sendReply(clientFd, "461 KICK :Wrong number of parameters", true);
        return;
    }

    std::string channelName = cmd.getParameters()[0];
    std::string kickedNick = cmd.getParameters()[1];
    std::string reason = (cmd.getParameters().size() > 2) ? cmd.getParameters()[2] : " :No reason given";

    // Check if the channel exists
    Channel* channel = _server.getChannel(channelName);
    if (!channel) {
        sendReply(clientFd, "403 " + channelName + " :No such channel", true);
        return;
    }
    

    // Check if the channel name is valid
    if (!isValidChannelName(channelName)) {
        sendReply(clientFd, "476 " + channelName + " :Bad Channel Mask", true);
        return;
    }

    // Check if the kicker is on the channel
    if (!channel->isMember(kicker)) {
        sendReply(clientFd, "442 " + channelName + " :You're not on that channel", true);
        return;
    }

    // Check if the kicker has channel operator privileges
    if (!channel->isOperator(kicker)) {
        sendReply(clientFd, "482 " + channelName + " :You're not channel operator", true);
        return;
    }

    // Check if the kicked user is on the channel
    Client* kicked = _server.getClientByNickname(kickedNick);
    if (!kicked || !channel->isMember(kicked)) {
        sendReply(clientFd, "441 " + kickedNick + " " + channelName + " :They aren't on that channel", true);
        return;
    }

    // All checks passed, proceed with kicking the user
    channel->removeMember(kicked);

    // Construct the kick message
    std::string kickMsg = ":" + kicker->getFullClientIdentifier() + " KICK " + channelName + " " + kickedNick + reason + "\r\n";

    // Send the kick message to all members of the channel, including the kicked user
    _server.broadcastToChannel(channelName, kickMsg);

    // Additionally, send the message to the kicked user
    _server.sendToClient(kicked->getFd(), kickMsg);

    // Log the kick action
    Logger::info("User " + kickedNick + " was kicked from " + channelName + " by " + kicker->getNickname() + ". Reason: " + reason);
}



bool CommandExecutor::isValidChannelName(const std::string& channelName) {
    // Check if the channel name is empty or too long
    if (channelName.empty() || channelName.length() > 50) {
        return false;
    }
    if (!isChannelSyntaxOk(channelName)){
        return false;
    }
    // Check for space, control G (ASCII 7), or comma
    for (size_t i = 1; i < channelName.length(); ++i) {
        char c = channelName[i];
        if (c == ' ' || c == 7 || c == ',') {
            return false;
        }
    }
    return true;
}

bool CommandExecutor::isChannelSyntaxOk(const std::string& channelName){
    if (channelName[0] != '&' && channelName[0] != '#' && channelName[0] != '+' && channelName[0] != '!') {
        return false;
    }
    return true;
}


void CommandExecutor::executePing(int clientFd, const Command& cmd){

    if (cmd.getParameters().size() < 1) {
        sendReply(clientFd, "461 PING :Wrong number of parameters", true);
        return;
    }
     std::string server = cmd.getParameters()[0];
    if (server[0] == ':')
        server = server.substr(1);
    if (cmd.getParameters().size() == 1 && (server != _server.getServerName())){
        sendReply(clientFd, "402 PING "+ server +" :No such server", true);
        return;
    }
        if (cmd.getParameters().size() == 2 && (cmd.getParameters()[1] != _server.getServerName())){
        sendReply(clientFd, "402 PING "+ server +" :No such server", true);
        return;
    }
     sendReply(clientFd, "PONG "+ _server.getServerName(), true);
}

void CommandExecutor::executeCap(int clientFd){

     sendReply(clientFd, "CAP * LS", true);
}


void CommandExecutor::executeWho(int clientFd, const Command& cmd) {
    if (cmd.getParameters().size() < 1) {
        sendReply(clientFd, "461 WHO :Wrong number of parameters", true);
        return;
    }

    std::string target = cmd.getParameters()[0];
    Client* requestingClient = _server.getClientByFd(clientFd);
    
    if (isChannelSyntaxOk(target)) {

        Channel* channel = _server.getChannel(target);
        if (channel) {
            const std::vector<Client*>& members = channel->getMembers();
            for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
                Client* member = *it;
                std::string flags = "H"; // Here, assuming all users are "Here" and not away
                if (channel->isOperator(member)) {
                    flags += "@";
                }
                sendReply(clientFd, "352 " + requestingClient->getNickname() + " " + 
                          target + " " + member->getUsername() + " " + 
                          member->getHostname() + " " + _server.getServerName() + " " + 
                          member->getNickname() + " " + flags + " :0 " + 
                          member->getRealname(), true);
            }
        }
    } else {
        // It's a user
        Client* targetClient = _server.getClientByNickname(target);
        if (targetClient) {
            sendReply(clientFd, "352 " + requestingClient->getNickname() + " * " + 
                      targetClient->getUsername() + " " + targetClient->getHostname() + " " + 
                      _server.getServerName() + " " + targetClient->getNickname() + 
                      " H :0 " + targetClient->getRealname(), true);
        }
    }

    // End of WHO list
    sendReply(clientFd, "315 " + requestingClient->getNickname() + " " + target + " :End of /WHO list", true);
}

void CommandExecutor::executeNotice(int clientFd, const Command& cmd) {
    if (cmd.getParameters().size() < 2) {
        // Do nothing; NOTICE should not generate error replies
        return;
    }

    std::string target = cmd.getParameters()[0];
    std::string message = cmd.getParameters()[1];
    Client* sender = _server.getClientByFd(clientFd);
    
    if (!sender) {
        Logger::error("Client not found for fd: " + to_string(clientFd));
        return;
    }

    // Check if the target is a channel
    if (isChannelSyntaxOk(target)) {
        Channel* channel = _server.getChannel(target);

        if (!channel || !channel->isMember(sender)) {
            // Do nothing; NOTICE should not generate error replies
            return;
        }

        Logger::info("Client is sending a notice to the channel...");

        // Construct the channel notice in the proper IRC format
        std::string channelNotice = ":" + sender->getFullClientIdentifier() + " NOTICE " + target + " :" + message + "\r\n";
        _server.broadcastToChannel(target, channelNotice, sender);

    } else {
        // Handle notices to a specific user
        Client* recipient = _server.getClientByNickname(target);
        
        if (!recipient) {
            // Do nothing; NOTICE should not generate error replies
            return;
        }

        Logger::info("Target: " + target + " | Recipient: " + recipient->getNickname());

        // Construct the private notice in the proper IRC format
        std::string privateNotice = ":" + sender->getFullClientIdentifier() + " NOTICE " + target + " :" + message + "\r\n";
        sendReply(recipient->getFd(), privateNotice, false);

        Logger::info("Notice sent to recipient: " + recipient->getNickname());
    }
}
