#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include <set>

class Client {
private:
    int _fd;                             // Socket file descriptor
    std::string _nickname;               // Client's nickname
    std::string _username;               // Client's username
    std::string _realname;               // Client's <realname> may contain space char
    std::string _hostname;               // Client's IP
    std::vector<std::string> _channels;  // Channels the client has joined
    bool _isPasswordSet;                 // Whether password is set
    bool _isUserSet;                     // Whether client has successfully sent the USER command during the IRC registration process.
    bool _passReceived; //wheter PASS command has been executed
    // bool _restricted;

public:
    Client(int fd);
    ~Client();

    // Getters
    int getFd() const;
    std::string getNickname() const;
    std::string getUsername() const;
    std::string getRealname() const;
    std::string getHostname() const;
    std::vector<std::string> getChannels() const;
    bool isPasswordSet() const;
    bool isUserSet() const;

    // Setters
    void setNickname(const std::string& nickname);
    void setUsername(const std::string& username);
    void setRealname(const std::string& realname);
    void setHostname(const std::string& hostname);
    void addChannel(const std::string& channel);
    void removeChannel(const std::string& channel);
    void setPassword(bool isSet);
    void setUser(bool isSet);

    
    bool isInChannel(const std::string& channel) const;
    std::string getFullClientIdentifier() const;
    bool isPassReceived() const;
    void setPassReceived(bool received);
};

#endif // CLIENT_HPP


//manca la forma canonica completa
