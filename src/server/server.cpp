
#include "server.hpp"



Server::Server(int port, const std::string& password) 
    : _serverSocket(-1),
      _port(port),
      _password(password),
      _serverName("ft_irc.com"),
      _clients(),
      _pollFds(),
      _cmdExecutor(NULL),
      _maxChannelsPerClient(3)
      {

    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket == -1) {
        Logger::error("Failed to create socket: " + std::string(strerror(errno)));
        throw std::runtime_error("Failed to create socket");
    }

    int opt = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        Logger::error("Failed to set socket options: " + std::string(strerror(errno)));
        throw std::runtime_error("Failed to set socket options");
    }

    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_port);

    if (bind(_serverSocket, (struct sockaddr*)&address, sizeof(address)) == -1) {
        Logger::error("Failed to bind to port :  "+ std::string(strerror(errno)));
        throw std::runtime_error("Failed to bind to port");
    }

    if (listen(_serverSocket, SOMAXCONN) == -1) {
        Logger::error("Failed to listen on socket: " + std::string(strerror(errno)));
        throw std::runtime_error("Failed to listen on socket");
    }

    if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) == -1) {
        Logger::error("Failed to set socket to non-blocking mode: " + std::string(strerror(errno)));
        throw std::runtime_error("Failed to set socket to non-blocking mode");
    }

    // Add server socket to poll set
    pollfd serverPollFd = {_serverSocket, POLLIN, 0};
    _pollFds.push_back(serverPollFd);

    _cmdExecutor = new CommandExecutor(*this);

    Logger::info("Server initialized on port " + std::to_string(_port));
}



//enhanced version: added Logger
Server::~Server() {
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete it->second;
    }
    delete _cmdExecutor;
    close(_serverSocket);
    Logger::info("Server shut down");
}


//enhanced version: added Logger
void Server::run() {
    Logger::info("Server started running");
    while (true) {
        int ret = poll(_pollFds.data(), _pollFds.size(), -1);
        if (ret == -1) {
            Logger::error("Poll failed: " + std::string(strerror(errno)));
            throw std::runtime_error("Poll failed");
        }
        Logger::info("A");
        for (size_t i = 0; i < _pollFds.size(); ++i) {
            if (_pollFds[i].revents & POLLIN) {
                if (_pollFds[i].fd == _serverSocket) {
                    Logger::info("B");
                    _acceptNewConnection();
                } else {
                    Logger::info("C");
                    _handleClientMessage(_pollFds[i].fd);
                }
            }
        }
    }
}



//enhanced version: added Logger
void Server::_acceptNewConnection() {

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        Logger::error("Failed to accept new connection: " + std::string(strerror(errno)));
        return;
    }

    std::string clientIP = _getIPAddress(clientAddr);
    if (clientIP == "Unknown") {
        close(clientSocket);
        return;
    }

    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
        Logger::error("Failed to set client socket to non-blocking mode: " + std::string(strerror(errno)));
        close(clientSocket);
        return;
    }

    // Add new client to our data structures
    Client* newClient = new Client(clientSocket);
    newClient->setHostname(clientIP);
    _clients[clientSocket] = newClient;

    Logger::debug("New client created with fd: " + std::to_string(clientSocket) + 
              ", password set: " + (newClient->isPasswordSet() ? "true" : "false"));
              
    pollfd clientPollFd = {clientSocket, POLLIN, 0};
    _pollFds.push_back(clientPollFd);

    Logger::info("New client connected from " + clientIP);
}

// void Server::_handleClientMessage(int clientFd) {
//     char buffer[1024];
//     ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer), 0);
//     if (bytesRead <= 0) {
//         if (bytesRead == 0) {
//             Logger::info("Client disconnected: " + std::to_string(clientFd));
//         } else {
//             Logger::error("Error reading from client " + std::to_string(clientFd) + ": " + std::string(strerror(errno)));
//         }
//         _removeClient(clientFd);
//         return;
//     }
//     Logger::info("D");
//     buffer[bytesRead] = '\0';
//     // Append to the client's buffer
//     _clients[clientFd]->appendToBuffer(std::string(buffer, bytesRead));

//     // Process complete commands
//     std::string& clientBuffer = _clients[clientFd]->getBuffer();
//     Logger::info("CLIENT BUFFER: " + clientBuffer );

//     size_t pos;
//     while ((pos = clientBuffer.find("\r\n")) != std::string::npos) {
//         std::string cmd = clientBuffer.substr(0, pos);
//         clientBuffer.erase(0, pos + 2);  // Remove processed command and \r\n

//         // Replace any newline characters with spaces
//         std::replace(cmd.begin(), cmd.end(), '\n', ' ');

//         // Check if the message (including \r\n) exceeds 512 bytes
//         if (cmd.length() + 2 > 512) {
//             Logger::warning("Received message too long from client " + std::to_string(clientFd));
//             sendToClient(clientFd, ":" + getServerName() + " 417 " + getClientByFd(clientFd)->getFullClientIdentifier() + " :Input line was too long\r\n");
//             continue;  // Process next command, if any
//         }
//         Logger::info("CICICI");
//         // Trim leading and trailing whitespace
//         cmd.erase(0, cmd.find_first_not_of(" \t"));
//         cmd.erase(cmd.find_last_not_of(" \t") + 1);

//         if (!cmd.empty()) {
//             Logger::debug("Received command from client " + std::to_string(clientFd) + ": " + cmd);

//             Command parsedCmd = CommandParser::parse(cmd);
//             if (parsedCmd.isValid()) {
//                 Logger::info("Parsed command: " + parsedCmd.toString());
//                 _cmdExecutor->executeCommand(clientFd, parsedCmd);
//             } else {
//                 Logger::warning("Invalid command received from client " + std::to_string(clientFd));
//                 sendToClient(clientFd, ":" + getServerName() + " 421 " + getClientByFd(clientFd)->getFullClientIdentifier() + " " + parsedCmd.getCommand() + " :Unknown command\r\n");
//             }
//         }
//     }

//     // Check if the remaining buffer exceeds the maximum length
//     if (clientBuffer.length() >= 510) { // 510 to allow for potential CR-LF
//         Logger::warning("Client " + std::to_string(clientFd) + " buffer exceeds maximum length of 512.");
//         sendToClient(clientFd, ":" + getServerName() + " :Message too long\r\n");
//         clientBuffer.clear(); 
//     }
// }


void Server::_handleClientMessage(int clientFd) {
    char buffer[1024];
    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        if (bytesRead == 0) {
            Logger::info("Client disconnected: " + std::to_string(clientFd));
        } else {
            Logger::error("Error reading from client " + std::to_string(clientFd) + ": " + std::string(strerror(errno)));
        }
        _removeClient(clientFd);
        return;
    }

    if (bytesRead > 0 && buffer[bytesRead - 1] == '\n') {
        bytesRead--;
    }

    buffer[bytesRead] = '\0';
    _clients[clientFd]->appendToBuffer(std::string(buffer, bytesRead));

    std::string& clientBuffer = _clients[clientFd]->getBuffer();
    Logger::info("CLIENT BUFFER: " + clientBuffer);


    size_t pos;
    while ((pos = clientBuffer.find("\r\n")) != std::string::npos) {
        std::string cmd = clientBuffer.substr(0, pos);
        clientBuffer.erase(0, pos + 2); // Remove processed command and \r\n

        
        // Check if the message (including \r\n) exceeds 512 bytes
        if (cmd.length() + 2 > 512) {
            Logger::warning("Received message too long from client " + std::to_string(clientFd));
            sendToClient(clientFd, ":" + getServerName() + " " + getClientByFd(clientFd)->getFullClientIdentifier() + " :Input line was too long\r\n");
            continue;
        }


        if (!cmd.empty()) {
            Logger::debug("Received command from client " + std::to_string(clientFd) + ": " + cmd);
            Command parsedCmd = CommandParser::parse(cmd);
            if (parsedCmd.isValid()) {
                Logger::info("Parsed command: " + parsedCmd.toString());
                _cmdExecutor->executeCommand(clientFd, parsedCmd);
            } else {
                Logger::warning("Invalid command received from client " + std::to_string(clientFd));
                sendToClient(clientFd, ":" + getServerName() + " [421] " + getClientByFd(clientFd)->getFullClientIdentifier() + " " + parsedCmd.getCommand() + " :Unknown command\r\n");
            }
        }
    }

    // Check if the remaining buffer exceeds the maximum length
    if (clientBuffer.length() >= 510) {  // 510 to allow for potential \r\n
        Logger::warning("Client " + std::to_string(clientFd) + " buffer exceeds maximum length of 512 bytes.");
        sendToClient(clientFd, ":" + getServerName() + " 417 " + getClientByFd(clientFd)->getFullClientIdentifier() + " :Input line was too long\r\n");
        clientBuffer.clear();
    }
}

//enhanced version: added Logger
void Server::_removeClient(int clientFd) {
    Logger::info("Removing client: " + std::to_string(clientFd));
    delete _clients[clientFd];
    _clients.erase(clientFd);
    close(clientFd);

    // Remove from _pollFds
    for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
        if (it->fd == clientFd) {
            _pollFds.erase(it);
            break;
        }
    }
}

void Server::broadcast(const std::string& message, int senderFd) {
    for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->first != senderFd) {
            sendToClient(it->first, message);
        }
    }
}

void Server::sendToClient(int clientFd, const std::string& message) {

    std::string formattedMessage = message;

    if (formattedMessage.substr(formattedMessage.length() - 2) != "\r\n") {
        formattedMessage += "\r\n";
    }
    ssize_t bytesSent = send(clientFd, message.c_str(), message.length(), 0);
    if (bytesSent == -1) {
        Logger::error("Failed to send message to client " + std::to_string(clientFd) + ": " + std::string(strerror(errno)));
    } else if (static_cast<size_t>(bytesSent) < message.length()) {
        Logger::warning("Incomplete message sent to client " + std::to_string(clientFd));
    } else {
        Logger::debug("Successfully sent " + std::to_string(bytesSent) + " bytes to client " + std::to_string(clientFd));
    }
}

int Server::getPort() const {
    return _port;
}

std::string Server::getPassword() const {
    return _password;
}

std::string Server::getServerName() const {
    return _serverName;
}

bool Server::isNicknameTaken(const std::string& nickname) const {
    for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->getNickname() == nickname) {
            return true;
        }
    }
    return false;
}

Client* Server::getClientByNickname(const std::string& nickname) {
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->getNickname() == nickname) {
            return it->second;
        }
    }
    return NULL;
}

Client* Server::getClientByFd(int fd) {
    std::map<int, Client*>::iterator it = _clients.find(fd);
    if (it != _clients.end()) {
        return it->second;
    }
    return NULL;
}

std::string Server::_getIPAddress(const struct sockaddr_in& clientAddr) const {
    char ipStr[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &(clientAddr.sin_addr), ipStr, INET_ADDRSTRLEN) == NULL) {
        Logger::error("Failed to convert IP address to string: " + std::string(strerror(errno)));
        return "Unknown";
    }
    return std::string(ipStr);
}

Channel* Server::getOrCreateChannel(const std::string& channelName, int clientFd) {
    std::map<std::string, Channel*>::iterator it = _channels.find(channelName);
    if (it != _channels.end()) {
        return it->second;
    }
    Channel* newChannel = new Channel(channelName);
    _channels[channelName] = newChannel;
    Client * client = getClientByFd(clientFd);
    newChannel->addOperator(client);
    return newChannel;
}


Channel* Server::getChannel(const std::string& channelName) {
    std::map<std::string, Channel*>::iterator it = _channels.find(channelName);
    if (it != _channels.end()) {
        return it->second;
    }
    return NULL;
}

void Server::broadcastToChannel(const std::string& channelName, const std::string& message, Client* excludeClient) {
    Channel* channel = getChannel(channelName);
    if (channel) {
        std::vector<Client*> members = channel->getMembers();
        for (std::vector<Client*>::iterator it = members.begin(); it != members.end(); ++it) {
            if (*it != excludeClient) {
                sendToClient((*it)->getFd(), message);
            }
        }
    }
}

bool Server::canJoinMoreChannels(const Client* client) const {
        return client->getChannels().size() < static_cast<size_t>(_maxChannelsPerClient);
    }

int Server::getMaxChannelsPerClient() const {
    return _maxChannelsPerClient;
}

Server::Server(const Server& other)
    : _serverSocket(other._serverSocket),
      _port(other._port),
      _password(other._password),
      _serverName(other._serverName),
      _clients(other._clients),
      _pollFds(other._pollFds),
      _cmdExecutor(NULL),
      _maxChannelsPerClient(other._maxChannelsPerClient)
      
  
{
    if (other._cmdExecutor) {
        _cmdExecutor = new CommandExecutor(*this);
    }
}

Server& Server::operator=(const Server& other)
{
    if (this != &other) {
        _port = other._port;
        _password = other._password;
        _serverName = other._serverName;
        _maxChannelsPerClient = other._maxChannelsPerClient;
        _serverSocket = other._serverSocket;
        _pollFds = other._pollFds;
        _clients = other._clients;

        delete _cmdExecutor;
        if (other._cmdExecutor) {
            _cmdExecutor = new CommandExecutor(*this);
        } else {
            _cmdExecutor = NULL;
        }
    }
    return *this;
}