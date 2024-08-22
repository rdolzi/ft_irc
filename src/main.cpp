#include "server/server.hpp"
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include "logger/logger.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }
    Logger::setLogLevel(Logger::DEBUG); 
    Logger::setLogFile("ircserver.log");
    int port;
    try {
        port = std::atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            throw std::out_of_range("Port number out of range");
        }
    } catch (const std::exception& e) {
        std::cerr << "Invalid port number: " << e.what() << std::endl;
        std::cerr << "<port>: A number between 0 and 65535" << std::endl;
        return 1;
    }

    std::string password = argv[2];

    try {
        Server server(port, password);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}


/*
per far partire il server
esempio:

./ircserver 10 psw

client:
nc localhost 10


PASS psw
NICK test
USER marco 0 * :Marco Rossi
*/ 


//kvIRC
//check if all messages sent from server have \r\n