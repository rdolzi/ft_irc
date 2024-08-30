#ifndef SERVER_HPP
#define SERVER_HPP

#include "../client/client.hpp"
#include "../logger/logger.hpp"
#include "./command/command.hpp"
#include "./channel/channel.hpp"
#include "./command/commandParser.hpp"
#include "./command/commandExecutor.hpp"
#include "../utils/server_utils.hpp"
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <cstring>

class CommandExecutor;

class Server {
private:
    int _serverSocket;
    int _port;
    std::string _password;
    std::string _serverName;
    std::map<int, Client*> _clients;
    std::vector<pollfd> _pollFds;
    CommandExecutor* _cmdExecutor;
    std::map<std::string, Channel*> _channels;
    int _maxChannelsPerClient;

    void _acceptNewConnection();
    void _handleClientMessage(int clientFd);
    void _removeClient(int clientFd);
    std::string _getIPAddress(const struct sockaddr_in& clientAddr) const;
    

public:
    Server(int port, const std::string& password);
    ~Server();
    Server(const Server& other);
    Server& operator=(const Server& other);

    void run();
    void broadcast(const std::string& message, int senderFd = -1);
    void sendToClient(int clientFd, const std::string& message);

    // Getters
    int getPort() const;
    std::string getPassword() const;
    std::string getServerName() const;
    
    bool isNicknameTaken(const std::string& nickname) const;
    Client* getClientByNickname(const std::string& nickname);
    Client* getClientByFd(int fd);
    Channel* getOrCreateChannel(const std::string& channelName, int clientFd);
    Channel* getChannel(const std::string& channelName);
    void broadcastToChannel(const std::string& channelName, const std::string& message, Client* excludeClient = NULL);
    bool canJoinMoreChannels(const Client* client) const;
    int getMaxChannelsPerClient() const;
    std::string generateUniqueId() const;
};

#endif // SERVER_HPP