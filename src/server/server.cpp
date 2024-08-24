
#include "server.hpp"



Server::Server(int port, const std::string& password) : _port(port), _password(password), _serverName("ft_irc.com") {
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
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_port);

    if (bind(_serverSocket, (struct sockaddr*)&address, sizeof(address)) == -1) {
        Logger::error("Failed to bind to port " + std::to_string(_port) + ": " + std::string(strerror(errno)));
        throw std::runtime_error("Failed to bind to port");
    }

    if (listen(_serverSocket, SOMAXCONN) == -1) {
        Logger::error("Failed to listen on socket: " + std::string(strerror(errno)));
        throw std::runtime_error("Failed to listen on socket");
    }

    // Set server socket to non-blocking mode
    int flags = fcntl(_serverSocket, F_GETFL, 0);
    if (flags == -1) {
        Logger::error("Failed to get socket flags: " + std::string(strerror(errno)));
        throw std::runtime_error("Failed to get socket flags");
    }
    if (fcntl(_serverSocket, F_SETFL, flags | O_NONBLOCK) == -1) {
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

        for (size_t i = 0; i < _pollFds.size(); ++i) {
            if (_pollFds[i].revents & POLLIN) {
                if (_pollFds[i].fd == _serverSocket) {
                    _acceptNewConnection();
                } else {
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

    // Set client socket to non-blocking mode
    int flags = fcntl(clientSocket, F_GETFL, 0);
    if (flags == -1) {
        Logger::error("Failed to get client socket flags: " + std::string(strerror(errno)));
        close(clientSocket);
        return;
    }
    if (fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK) == -1) {
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

//executeCommand & sendToClient implemented
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

    buffer[bytesRead] = '\0';
    std::string message(buffer);
    Logger::debug("Received message from client " + std::to_string(clientFd) + ": " + message);

    Command cmd = CommandParser::parse(message);
    if (cmd.isValid()) {
        Logger::info("Parsed command: " + cmd.toString());
        _cmdExecutor->executeCommand(clientFd, cmd);
    } else {
        Logger::warning("Invalid command received from client " + std::to_string(clientFd));
        sendToClient(clientFd, ": 421 * " + cmd.getCommand() + " :Unknown command\r\n");
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
    // client->setOperator(true);
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