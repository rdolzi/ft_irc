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

    // Check for too many parameters
    if (cmd.getParameters().size() > 15) {
        sendReply(clientFd, command + " :Too many parameters. Max is set to 15.");
        return;
    }

    if (command == "PING") {
        executePing(clientFd, cmd);
        return;
    }

    if (command == "PASS") {
        executePass(clientFd, cmd);
        return;
    }

    // Check if PASS has been received before allowing other commands
    if (!client->isPasswordSet()) {
        sendReply(clientFd, " * :Password required");
        return;
    }

    if (!isRegistered(client) && command != "PASS" && command != "NICK" && command != "USER") {
        sendReply(clientFd, "[451] * :You have not registered");
        return;
    }
    

    if (command == "NICK") {
       executeNick(clientFd, cmd);
    } else if (command == "USER") {
        executeUser(clientFd, cmd);
    } else if (command == "JOIN") {
        executeJoin(clientFd, cmd);
    } else if (command == "PRIVMSG") {
        executePrivmsg(clientFd, cmd);
    }  else if (command == "MODE") {
        executeMode(clientFd, cmd);
    } else if (command == "TOPIC") {
        executeTopic(clientFd, cmd);
    } else if (command == "INVITE") {
        executeInvite(clientFd, cmd);
    } else if (command == "KICK") {
        executeKick(clientFd, cmd);
    }else {
        Logger::warning("Unimplemented command: " + command);
        sendReply(clientFd, "[421] * " + command + " :Unknown command");
    }
}


void CommandExecutor::executePass(int clientFd, const Command& cmd) {
    Client* client = _server.getClientByFd(clientFd);
    
    Logger::debug("Executing PASS command. Client password set: " + 
                  std::string(client->isPasswordSet() ? "true" : "false"));

    if (client->isPasswordSet()) {
        sendReply(clientFd, "[462] * :Unauthorized command (already registered)");
        Logger::debug("Sent [462] 'already registered' reply");
        return;
    }

    if (cmd.getParameters().empty()) {
        sendReply(clientFd, "[461] PASS :Not enough parameters");
        Logger::debug("Sent [461] 'not enough parameters' reply");
        return;
    }

    std::string password = cmd.getParameters()[0];
    Logger::info("Param[0]: "+ password + " actual psw: " +_server.getPassword());
    if (password == _server.getPassword()) {
        client->setPassword(true);
        //sendReply(clientFd, ":Password accepted");
        Logger::debug("Client "+ std::to_string(clientFd)  + " set password correctly.");
    } else {
        sendReply(clientFd, "[464] * :Password incorrect");
        Logger::debug("Sent [464] 'password incorrect' reply");
    }
}


void CommandExecutor::executeNick(int clientFd, const Command& cmd) {
    Client* client = _server.getClientByFd(clientFd);
    Logger::debug("Executing NICK command. Current nickname: " + client->getNickname());

    if (cmd.getParameters().size() != 1) {
        sendReply(clientFd, "[431] * :No nickname given");
        Logger::debug("Sent [431] 'no nickname given' reply");
        return;
    }

    std::string newNick = cmd.getParameters()[0];

    if (!isValidNickname(newNick)) {
        sendReply(clientFd, "[432] " + newNick + " :Erroneous nickname");
        Logger::debug("Sent [432] 'erroneous nickname' reply");
        return;
    }

    if (_server.isNicknameTaken(newNick)) {
        sendReply(clientFd, "[433] " + newNick + " :Nickname is already in use");
        Logger::debug("Sent 'nickname in use' reply");
        return;
    }

    std::string oldNick = client->getNickname();
    std::string oldClientIdentifier = client->getFullClientIdentifier();
    client->setNickname(newNick);

    if (oldNick.empty() && isRegistered(client)) {
        Logger::debug("Nickname set for the first time: " + newNick);
        sendReply(clientFd, "[001] " + newNick + " :Welcome to the Internet Relay Network " + client->getFullClientIdentifier());
        Logger::debug("[001] Registration complete, sent welcome message");
    } else {
        _server.broadcast(":" + oldClientIdentifier + " NICK " + newNick + "\r\n", clientFd);
        Logger::debug("Nickname changed from " + oldNick + " to " + newNick);
    }
}


void CommandExecutor::executeUser(int clientFd, const Command& cmd) {
    Client* client = _server.getClientByFd(clientFd);
    Logger::debug("Executing USER command. Is user set: " + std::string(client->isUserSet() ? "true" : "false"));

    if (client->isUserSet()) {
        sendReply(clientFd, "[462] :Unauthorized command (already registered)");
        Logger::debug("Sent [462] 'already registered' reply");
        return;
    }

    if (cmd.getParameters().size() < 4) {
        sendReply(clientFd, "[461] USER :Not enough parameters");
        Logger::debug("Sent [461] 'not enough parameters' reply");
        return;
    }

    std::string username = cmd.getParameters()[0];
    //std::string realname = cmd.getParameters()[3];
    std::string realname = (cmd.getParameters()[3][0] == ':' ? cmd.getParameters()[3].substr(1) : cmd.getParameters()[3]);


    client->setUsername(username);
    client->setRealname(realname);
    client->setUser(true);

    Logger::debug("User info set - Username: " + username + ", Realname: " + realname);

    if (isRegistered(client)) {
        sendReply(clientFd, "[001] " + client->getNickname() + " :Welcome to the Internet Relay Network " + client->getFullClientIdentifier());
        Logger::debug("Registration complete, sent welcome message");
    }
}


void CommandExecutor::executeJoin(int clientFd, const Command& cmd) {
    if (cmd.getParameters().empty()) {
        sendReply(clientFd, "[461] JOIN :Not enough parameters");
        Logger::debug("Sent [461] 'not enough parameters' reply");
        return;
    }

    std::string channelName = cmd.getParameters()[0];
    std::string key = (cmd.getParameters().size() > 1) ? cmd.getParameters()[1] : "";
    Client* client = _server.getClientByFd(clientFd);


    // Check if the channel name is valid
    if (!isValidChannelName(channelName)) {
        sendReply(clientFd, "[403] " + channelName + " :No such channel");
         Logger::debug("Sent [403] 'No such channel' reply");
        return;
    }


    // Check if the client has reached the channel join limit
    if (!_server.canJoinMoreChannels(client)) {
        sendReply(clientFd, "[405] " + channelName + " :You have joined too many channels");
        return;
    }

    Channel* channel = _server.getOrCreateChannel(channelName, clientFd);
    if (!channel) {
        sendReply(clientFd, "[403] " + channelName + " :No such channel");
        Logger::debug("Sent [403] 'No such channel' reply");
        return;
    }

    // Check invite-only status
    if (channel->isInviteOnly() && !channel->isInvited(client)) {
        sendReply(clientFd, "[473] " + channelName + " :Cannot join channel (+i)");
        Logger::debug("Sent [473] 'Cannot join channel (+i)'");
        return;
    }

    // Check channel key
    if (!channel->checkKey(key)) {
        sendReply(clientFd, "[475] " + channelName + " :Cannot join channel (+k)");
        Logger::debug("Sent [475] 'Cannot join channel (+k)'");
        return;
    }

    // Check user limit
    if (channel->isFull()) {
        sendReply(clientFd, "[471] " + channelName + " :Cannot join channel (+l)");
        Logger::debug("Sent [471] 'Cannot join channel (+l)'");
        return;
    }

   

    // Broadcast join message to all clients in the channel
    _server.broadcast(":" + client->getFullClientIdentifier() + " JOIN :" + channelName + "\r\n");

    // Send channel topic
    std::string topic = channel->getTopic();
    if (!topic.empty()) {
        sendReply(clientFd, "[332] " + client->getNickname() + " " + channelName + " :" + topic);
    } 
    else {
        sendReply(clientFd, "[331] " + client->getNickname() + " " + channelName + " :No topic is set");
    }

    
     if (channel->getMembers().size() == 0){
            Logger::info("Channel size is 0, client is now the operator!");
            channel->addOperator(client); 
    }

    client->addChannel(channelName);

     // Add member to channel
    if (!channel->addMember(client, key)) {
        Logger::error("Failed to add member to channel after all checks passed");
        return;
    }

    // Send names list
    std::string names = channel->getNames();
    sendReply(clientFd, "[353] " + client->getNickname() + " = " + channelName + " :" + names);
    sendReply(clientFd, "[366] " + client->getNickname() + " " + channelName + " :End of /NAMES list");
    
}


void CommandExecutor::executePrivmsg(int clientFd, const Command& cmd) {
    if (cmd.getParameters().size() < 2) {
        sendReply(clientFd, "[411] :No recipient given (PRIVMSG)");
        return;
    }

    std::string target = cmd.getParameters()[0];
    std::string message = cmd.getParameters()[1];
    Client* sender = _server.getClientByFd(clientFd);

    
    if (isChannelSyntaxOk(target)) {
        // Check if the channel name is valid
        if (!isValidChannelName(target)) {
        sendReply(clientFd, "[403] " + target + " :No such channel");
         Logger::debug("Sent [403] 'No such channel' reply");
        return;
        }
        Channel *channel = _server.getChannel(target);
        if (!channel->isMember(sender)){
            sendReply(clientFd, "[404] " + target + " ::Cannot send to channel");
         Logger::debug("Client in not a member of channel.Sent [404] ':Cannot send to channel'");
        }
        Logger::info("client is sending message to channel...");
        std::string channelMessage =  ":" + sender->getFullClientIdentifier() + " PRIVMSG " + target + " :" + message;
        _server.broadcastToChannel(target, channelMessage);
        // Channel message
        // TODO: Implement channel messaging
        // Check if channel exists, if user is in channel, if user can speak in channel
    } else {
        Client* recipient = _server.getClientByNickname(target);
        Logger::info("targer: "+target + "| client: "+ recipient->getNickname());
        if (recipient) {
            sendReply(recipient->getFd(), ":" + sender->getFullClientIdentifier() + " PRIVMSG " + target + " :" + message);
        } else { //?
            sendReply(clientFd, "[401] " + sender->getNickname() + " " + target + " :No such nick/channel");
        }
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

void CommandExecutor::sendReply(int clientFd, const std::string& reply) const {
    std::string formattedReply = ":" + _server.getServerName() + " " + reply + "\r\n";
    Logger::debug("Sending reply to client " + std::to_string(clientFd) + ": " + formattedReply);
    _server.sendToClient(clientFd, formattedReply);
}


void CommandExecutor::executeMode(int clientFd, const Command& cmd) {
    Logger::info("A");
    if (cmd.getParameters().size() < 2 || cmd.getParameters().size() > 3) {
        sendReply(clientFd, "[461] MODE :Not enough parameters");
        return;
    }
    Logger::info("1");
    std::string channel = cmd.getParameters()[0];
    std::string modestring = cmd.getParameters()[1];

    std::vector<std::string> args;
     for (size_t i = 0; i < cmd.getParameters().size(); ++i) {
        Logger::info("Parameter " + std::to_string(i) + ": '" + cmd.getParameters()[i] + "'");
        args.push_back(cmd.getParameters()[i]);
    }
    Logger::info("2");
    // std::vector<std::string> args(cmd.getParameters().begin() + 2, cmd.getParameters().end());
    Logger::info("3");
    Logger::info("B");
    // Check if the channel name is valid
    if (!isValidChannelName(channel)) {
        sendReply(clientFd, "[476] " + channel + " :Bad Channel Mask");
        return;
    }
    handleChannelMode(clientFd, channel, modestring, args);
}

// void CommandExecutor::executeMode(int clientFd, const Command& cmd) {
//     Logger::debug("Entering executeMode. ClientFd: " + std::to_string(clientFd));

//     try {
//         if (cmd.getParameters().size() < 2) {
//             sendReply(clientFd, "[461] MODE :Not enough parameters");
//             return;
//         }

//         std::string target = cmd.getParameters()[0];
//         std::string modestring = cmd.getParameters()[1];
//         std::vector<std::string> args(cmd.getParameters().begin() + 2, cmd.getParameters().end());

//         Logger::debug("Mode target: " + target + ", modestring: " + modestring);

//         // Check if the channel exists
//         Channel* channel = _server.getChannel(target);
//         if (!channel) {
//             Logger::error("Channel not found: " + target);
//             sendReply(clientFd, "[403] " + target + " :No such channel");
//             return;
//         }

//         // Check if the client exists
//         Client* client = _server.getClientByFd(clientFd);
//         if (!client) {
//             Logger::error("Client not found for fd: " + std::to_string(clientFd));
//             return;
//         }

//         // Check if the client is an operator
//         if (!channel->isOperator(client)) {
//             Logger::info("Non-operator attempted to change mode. Client: " + client->getNickname());
//             sendReply(clientFd, "[482] " + client->getFullClientIdentifier() + " :You're not channel operator");
//             return;
//         }

//         //handleChannelMode(clientFd, target, modestring, args);

//     } catch (const std::exception& e) {
//         Logger::error("Exception in executeMode: " + std::string(e.what()));
//         sendReply(clientFd, "[500] :Internal server error");
//     } catch (...) {
//         Logger::error("Unknown exception in executeMode");
//         sendReply(clientFd, "[500] :Internal server error");
//     }

//     Logger::debug("Exiting executeMode");
// }


void CommandExecutor::handleChannelMode(int clientFd, const std::string& channelName, const std::string& modestring, const std::vector<std::string>& args) {
    // 1. Validate the channel
    Channel* channel = _server.getChannel(channelName);
    if (!channel) {
        sendReply(clientFd, "[403] " + channelName + " :No such channel");
        return;
    }
    Logger::info("C");
    // 2. Initialize variables for processing modes
    Client* client = _server.getClientByFd(clientFd);

    if (!client) {
        Logger::error("Client not found in handleChannelMode for fd: " + std::to_string(clientFd));
        return;
    }

    bool adding = true;
    size_t argIndex = 0;
    std::string modeChanges;
    std::string modeArgs;
    int paramModeCount = 0;

    if (!channel->isOperator(client)){
        sendReply(clientFd, "[481] "+client->getFullClientIdentifier()+" :Permission Denied - You do not have the required privileges");
        return;
    }
    Logger::info("D");
    // 3. Process each character in the modestring
    for (size_t i = 0; i < modestring.length(); ++i) {
        Logger::info("In for loop, i: "+std::to_string(i));
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
            Logger::info("F");
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
                        channel->setKey(args[argIndex]);
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
                    Logger::info("G");
                    Logger::info("In case o");
                    if (args.size() == 3) {
                        Client* targetClient = _server.getClientByNickname(args[2]);
                        if (!targetClient) {
                            Logger::error("Client not found in handleChannelMode for fd: " + std::to_string(clientFd));
                            return;
                        }
                        Logger::info("H");
                        Logger::info("targetClient: "+ targetClient->getNickname());
                        if (targetClient) {
                            if (adding) {
                                channel->addOperator(targetClient);
                            } else {
                                channel->removeOperator(targetClient);
                            }
                            modeChanges += mode;
                            modeArgs += " " + args[argIndex];
                            paramModeCount++;
                        }
                        argIndex++;
                    }
                    break;
                case 'l': // User limit
                    if (adding && argIndex < args.size()) {
                        channel->setUserLimit(std::stoi(args[argIndex]));
                        modeChanges += mode;
                        modeArgs += " " + args[argIndex];
                        paramModeCount++;
                    } else if (!adding) {
                        channel->removeUserLimit();
                        modeChanges += mode;
                    }
                    argIndex++;
                    break;
                default:
                    sendReply(clientFd, "[501] " + std::string(1, mode) + " :is unknown mode char to me");
                    argIndex++;
                    break;
            }
        }
    }

    // 4. Build the mode change string && broadcast
    if (!modeChanges.empty()) {
        std::string modeMessage = ":" + client->getNickname() + " MODE " + channelName + " " + modeChanges + modeArgs;
        _server.broadcastToChannel(channelName, modeMessage);
    }
}




void CommandExecutor::executeTopic(int clientFd, const Command& cmd) {
    Client* client = _server.getClientByFd(clientFd);
    if (cmd.getParameters().empty() || cmd.getParameters().size() > 2) {
        sendReply(clientFd, "[461] TOPIC :Not enough parameters");
        return;
    }

    // dal index 1
    std::string channelName = cmd.getParameters()[0];
    Channel* channel = _server.getChannel(channelName);
    Logger::info(">>>>>> "+ channelName);
    if (!channel) {
        return;
    }

    if (!channel->isMember(client)) {
        sendReply(clientFd, "[442] " + channelName + " :You're not on that channel");
        return;
    }


     if (!channel->isOperator(client)) {
        sendReply(clientFd, "[482] " + channelName + " :You're not channel operator");
        return;
    }
    
    if (channel->isTopicRestricted()) {
        sendReply(clientFd, " " + channelName + " :topic is restricted (+t)");
        return;
    }

    // If there's no topic provided, we're querying the current topic
    if (cmd.getParameters().size() == 1) {
        std::string currentTopic = channel->getTopic();
        if (currentTopic.empty()) {
            sendReply(clientFd, "[331] " + channelName + " :No topic is set");
        } else {
            sendReply(clientFd, "[332] " + channelName + " :" + currentTopic);
        }
        return;
    }

    
    // if args == 2, 
    std::string newTopic = cmd.getParameters()[1];
    channel->setTopic(newTopic);

    // Broadcast the new topic to all members of the channel
    std::string topicMessage = ":" + client->getFullClientIdentifier() + " TOPIC " + channelName + " :" + newTopic;
    _server.broadcastToChannel(channelName, topicMessage);

}

void CommandExecutor::executeInvite(int clientFd, const Command& cmd) {
    Client* inviter = _server.getClientByFd(clientFd);


    // Check for correct number of parameters
    if (cmd.getParameters().size() != 2) {
        sendReply(clientFd, "[461] INVITE :Not enough parameters");
        return;
    }

    std::string inviterNick = cmd.getParameters()[0];
    std::string channelName = cmd.getParameters()[1];

    // Check if the invitee exists
    Client* invitee = _server.getClientByNickname(inviterNick);
    if (!invitee) {
        sendReply(clientFd, "[401] " + inviterNick + " :No such nick/channel");
        return;
    }

    // Check if the channel exists
    Channel* channel = _server.getChannel(channelName);
    if (!channel) {
        sendReply(clientFd, "[403] " + channelName + " :No such channel");
        return;
    }
    
    // Check if the inviter is on the channel
    if (!channel->isMember(inviter)) {
        sendReply(clientFd, "[442] " + channelName + " :You're not on that channel");
        return;
    }

    if (!channel->isOperator(inviter)){
    sendReply(clientFd, "[482] "+inviter->getFullClientIdentifier()+" "+channelName+ " :You're not channel operator");
    return;
    }

    // Check if the invitee is already on the channel
    if (channel->isMember(invitee)) {
        sendReply(clientFd, "[443] " + inviterNick + " " + channelName + " :is already on channel");
        return;
    }

    // Check if the channel is invite-only and if the inviter has the necessary privileges
    if (!channel->isOperator(inviter)) {
        sendReply(clientFd, "[482] "+inviter->getFullClientIdentifier()+" "+channelName+ " :You're not channel operator");
        return;
    }

    // All checks passed, proceed with the invitation
    channel->inviteClient(invitee);

    // Send confirmation to the inviter
    sendReply(clientFd, "[341] " + inviterNick + " " + channelName);

    // Send invitation to the invitee
    std::string inviteMsg = ":" + inviter->getFullClientIdentifier() + " INVITE " + inviterNick + " :" + channelName + "\r\n";
    _server.sendToClient(invitee->getFd(), inviteMsg);

}


void CommandExecutor::executeKick(int clientFd, const Command& cmd) {
    Client* kicker = _server.getClientByFd(clientFd);

    // Check for correct number of parameters
    if (cmd.getParameters().size() < 2 || cmd.getParameters().size() > 3) {
        sendReply(clientFd, "[461] KICK :Not enough parameters");
        return;
    }

    std::string channelName = cmd.getParameters()[0];
    std::string kickedNick = cmd.getParameters()[1];
    std::string reason = (cmd.getParameters().size() > 2) ? cmd.getParameters()[2] : "No reason given";

    // Check if the channel exists
    Channel* channel = _server.getChannel(channelName);
    if (!channel) {
        sendReply(clientFd, "[403] " + channelName + " :No such channel");
        return;
    }
    

    // Check if the channel name is valid
    if (!isValidChannelName(channelName)) {
        sendReply(clientFd, "[476] " + channelName + " :Bad Channel Mask");
        return;
    }

    // Check if the kicker is on the channel
    if (!channel->isMember(kicker)) {
        sendReply(clientFd, "[442] " + channelName + " :You're not on that channel");
        return;
    }

    // Check if the kicker has channel operator privileges
    if (!channel->isOperator(kicker)) {
        sendReply(clientFd, "[482] " + channelName + " :You're not channel operator");
        return;
    }

    // Check if the kicked user is on the channel
    Client* kicked = _server.getClientByNickname(kickedNick);
    if (!kicked || !channel->isMember(kicked)) {
        sendReply(clientFd, "[441] " + kickedNick + " " + channelName + " :They aren't on that channel");
        return;
    }

    // All checks passed, proceed with kicking the user
    channel->removeMember(kicked);

    // Construct the kick message
    std::string kickMsg = ":" + kicker->getFullClientIdentifier() + " KICK " + channelName + " " + kickedNick + " :" + reason + "\r\n";

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

    // // Check if the first character is valid
    // if (channelName[0] != '&' && channelName[0] != '#' && channelName[0] != '+' && channelName[0] != '!') {
    //     return false;
    // }
    
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
        sendReply(clientFd, "[461] PING :Not enough parameters");
        return;
    }
    if (cmd.getParameters()[0] != _server.getServerName()){
        sendReply(clientFd, "[402] PING "+ cmd.getParameters()[0] +" :No such server");
        return;
    }
     sendReply(clientFd, "PONG "+ _server.getServerName());
}


