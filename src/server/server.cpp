
#include "server.hpp"

Server::Server(int port, const std::string& password) : _port(port), _password(password) {
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    int opt = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        throw std::runtime_error("Failed to set socket options");
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_port);

    if (bind(_serverSocket, (struct sockaddr*)&address, sizeof(address)) == -1) {
        throw std::runtime_error("Failed to bind to port");
    }

    if (listen(_serverSocket, SOMAXCONN) == -1) {
        throw std::runtime_error("Failed to listen on socket");
    }

    // Set server socket to non-blocking mode
    int flags = fcntl(_serverSocket, F_GETFL, 0);
    fcntl(_serverSocket, F_SETFL, flags | O_NONBLOCK);

    // Add server socket to poll set
    pollfd serverPollFd = {_serverSocket, POLLIN, 0};
    _pollFds.push_back(serverPollFd);
}

Server::~Server() {
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete it->second;
    }
    close(_serverSocket);
}

void Server::run() {
    while (true) {
        int ret = poll(_pollFds.data(), _pollFds.size(), -1);
        if (ret == -1) {
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

void Server::_acceptNewConnection() {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        std::cerr << "Failed to accept new connection" << std::endl;
        return;
    }

    // Set client socket to non-blocking mode
    int flags = fcntl(clientSocket, F_GETFL, 0);
    fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);

    // Add new client to our data structures
    Client* newClient = new Client(clientSocket);
    _clients[clientSocket] = newClient;

    pollfd clientPollFd = {clientSocket, POLLIN, 0};
    _pollFds.push_back(clientPollFd);

    std::cout << "New client connected" << std::endl;
}

void Server::_handleClientMessage(int clientFd) {
    char buffer[1024];
    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        if (bytesRead == 0) {
            std::cout << "Client disconnected" << std::endl;
        } else {
            std::cerr << "Error reading from client" << std::endl;
        }
        _removeClient(clientFd);
        return;
    }

    buffer[bytesRead] = '\0';
    std::cout << "Received message from client: " << buffer << std::endl;

    // TODO: Implement command parsing and execution here
}

void Server::_removeClient(int clientFd) {
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
    send(clientFd, message.c_str(), message.length(), 0);
}

int Server::getPort() const {
    return _port;
}

std::string Server::getPassword() const {
    return _password;
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