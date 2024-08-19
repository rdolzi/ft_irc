# ft_irc - Internet Relay Chat Server

## Table of Contents
- [Project Description](#project-description)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Architecture](#architecture)
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

For more detailed instructions on connecting with HexChat, see our [HexChat Connection Guide](docs/user-guide/how-hexchat-connect.md).

## Project Structure

Our IRC server follows a modular architecture with the following key components:

- Server: Manages client connections and message routing
- Client: Represents a connected user
- Command Executor: Interprets and executes IRC commands
- Logger: Provides a centralized logging system


## Authors

- [rdolzi](https://github.com/rdolzi)
- [Fbiondo00](https://github.com/Fbiondo00)


## References

References
For a deeper understanding of the IRC protocol, refer to the following RFCs:

<a href="https://tools.ietf.org/html/rfc1459" target="_blank">RFC 1459</a> - Internet Relay Chat Protocol
<a href="https://tools.ietf.org/html/rfc2810" target="_blank">RFC 2810</a> - Internet Relay Chat: Architecture
<a href="https://tools.ietf.org/html/rfc2811" target="_blank">RFC 2811</a> - Internet Relay Chat: Channel Management
<a href="https://tools.ietf.org/html/rfc2812" target="_blank">RFC 2812</a> - Internet Relay Chat: Client Protocol
<a href="https://tools.ietf.org/html/rfc2813" target="_blank">RFC 2813</a> - Internet Relay Chat: Server Protocol
<a href="https://tools.ietf.org/html/rfc7194" target="_blank">RFC 7194</a> - Default Port for Internet Relay Chat (IRC) via TLS/SSLl

For user documentation and guides, please check the [docs](./docs) directory.
