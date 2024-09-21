# ft_irc - Internet Relay Chat Server

## Table of Contents
- [Project Description](#project-description)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Authors](#authors)
- [References](#references)

## Project Description

ft_irc is an implementation of an Internet Relay Chat (IRC) server in C++98. This project aims to create a fully functional IRC server that complies with the IRC protocol as defined in RFC 2812, allowing users to communicate in real-time through channels and private messages.

## Features

- Supports multiple client connections
- Implements core IRC commands (JOIN, PRIVMSG, NICK, etc.)
- Channel management
- Private messaging
- User authentication
- Operator privileges
- Compliant with C++98 standard

## Installation

### Prerequisites

- C++ compiler with C++98 support
- Make

### Building the Project

1. Clone the repository:
   ```
   git clone https://github.com/rdolzi/ft_irc.git
   cd ft_irc
   ```

2. Compile the project:
   ```
   make
   ```

This will create an executable named `ircserv`.

## Usage

To start the IRC server:

```
./ircserv <port> <password>
```

- `<port>`: The port number on which the server will listen for incoming connections.
- `<password>`: The password that clients need to provide to connect to the server.

### Connecting with a Client

You can use any IRC client to connect to the server. For example, using HexChat:

1. Open HexChat
2. Go to `Network List` -> `Add`
3. Set the server address to `localhost` and the port to the one you specified when starting the server
4. In the `Connect Commands` field, add: `/PASS <password>`
5. Click `Connect`

For complete user guide click here:  [Comprehensive Irc User Guide](docs/user-guide/comprehensive-irc-user-guide.md).

## Project Structure

Our IRC server follows a modular architecture with the following key components:

- Server: Manages client connections, channels, command execution, message routing
- Client: Represents a connected user
- Command Executor: Interprets and executes IRC commands
- Logger: Provides a centralized logging system


## References

For a deeper understanding of the IRC protocol, refer to the following RFCs:

- [RFC 1459 - Internet Relay Chat Protocol](https://tools.ietf.org/html/rfc1459)

- [RFC 2810 - Internet Relay Chat: Architecture](https://tools.ietf.org/html/rfc2810)

- [RFC 2811 - Internet Relay Chat: Channel Management](https://tools.ietf.org/html/rfc2811)

- [RFC 2812 - Internet Relay Chat: Client Protocol](https://tools.ietf.org/html/rfc2812)

- [RFC 2813 - Internet Relay Chat: Server Protocol](https://tools.ietf.org/html/rfc2813)

- [RFC 7194 - Default Port for Internet Relay Chat (IRC) via TLS/SSL](https://tools.ietf.org/html/rfc7194)


For user documentation and guides, please check the [docs](./docs) directory.
