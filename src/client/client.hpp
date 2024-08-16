#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

class Client {
private:
    int _fd;                             // Socket file descriptor
    std::string _nickname;               // Client's nickname
    std::string _username;               // Client's username
    std::vector<std::string> _channels;  // Channels the client has joined
    bool _isOperator;                    // Whether the client is an operator

public:
    Client(int fd);
    ~Client();

    // Getters
    int getFd() const;
    std::string getNickname() const;
    std::string getUsername() const;
    std::vector<std::string> getChannels() const;
    bool isOperator() const;

    // Setters
    void setNickname(const std::string& nickname);
    void setUsername(const std::string& username);
    void addChannel(const std::string& channel);
    void removeChannel(const std::string& channel);
    void setOperator(bool isOperator);

    
    bool isInChannel(const std::string& channel) const;
};

#endif // CLIENT_HPP


//manca la forma canonica completa