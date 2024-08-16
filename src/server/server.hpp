#ifndef SERVER_HPP
#define SERVER_HPP

#include "../client/client.hpp"
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <cstring>

class Server {
private:
    int _serverSocket;
    int _port;
    std::string _password;
    std::map<int, Client*> _clients;
    std::vector<pollfd> _pollFds;

    void _acceptNewConnection();
    void _handleClientMessage(int clientFd);
    void _removeClient(int clientFd);

public:
    Server(int port, const std::string& password);
    ~Server();

    void run();
    void broadcast(const std::string& message, int senderFd = -1);
    void sendToClient(int clientFd, const std::string& message);

    // Getters
    int getPort() const;
    std::string getPassword() const;

    
    bool isNicknameTaken(const std::string& nickname) const;
    Client* getClientByNickname(const std::string& nickname);
};

#endif // SERVER_HPP