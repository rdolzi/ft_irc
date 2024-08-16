#include "server/server.hpp"
#include <iostream>
#include <cstdlib>
#include <stdexcept>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }

    int port;
    try {
        port = std::atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            throw std::out_of_range("Port number out of range");
        }
    } catch (const std::exception& e) {
        std::cerr << "Invalid port number: " << e.what() << std::endl;
        std::cerr << "<port>: A number between 1024 and 65535" << std::endl;
        return 1;
    }

    std::string password = argv[2];

    try {
        Server server(port, password);
        std::cout << "IRC Server started on port " << port << std::endl;
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}