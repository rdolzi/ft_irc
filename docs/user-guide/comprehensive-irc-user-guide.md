# Updated Comprehensive IRC User Guide

## Introduction

This document provides detailed instructions on how to use our IRC (Internet Relay Chat) server. Our server implements a subset of the standard IRC protocols, allowing real-time chat with other users. This implementation focuses on core IRC functionality and channel operator commands.

Please note that while this guide covers a wide range of IRC commands and features, not all of them are currently implemented in our server. We will clearly indicate which features are available and which are not.

## Connecting to the Server

To connect to the server, you'll need an IRC client. Some popular IRC clients include:

- HexChat (Windows, Linux, macOS)
- mIRC (Windows)
- irssi (Linux, macOS)
- Textual (macOS)

**Important Note**: While various clients are mentioned, our server has been specifically tested and guaranteed for full compatibility with HexChat only, as per project requirements.

Once you have a client installed, use the following details to connect and register:

1. Establish a TCP connection to the server
2. Send the PASS command (if a server password is set)
3. Send the NICK command to set the user's nickname
4. Send the USER command to provide additional user information

## Available Commands and Actions

This section outlines all actions a client can perform on our IRC server, both as a regular user and as an operator. Each action is accompanied by examples, associated functionality, and information on repeatability and specific behaviors.

## Messages

### IRC Server and Client Identifiers

#### 1. RFC 2812 Section 2.3.1 Excerpt

The following is an excerpt from RFC 2812, Section 2.3.1, which defines the message format in Augmented BNF:

```
prefix     =  servername / ( nickname [ [ "!" user ] "@" host ] )
```

#### 2. Server Identifier

- Format: `servername`
- Example: `:irc.example.com`

The server identifier is simply the name of the server. It's used as the prefix in messages originating from the server, such as numeric replies or server-to-client messages like PING.

Usage examples:
1. Numeric reply: `:irc.example.com 001 nickname :Welcome to the IRC Network`
2. PING message: `:irc.example.com PING :irc.example.com`

#### 3. Client Identifier

- Full Format: `nickname!username@hostname`
- Partial Formats: 
  - `nickname`
  - `nickname@hostname`
  - `nickname!username`

The client identifier can appear in full or partial forms, depending on the context and available information.

Usage examples:
1. Full identifier: `:john!jdoe@example.com PRIVMSG #channel :Hello, world!`
2. Partial (nickname only): `:john QUIT :Leaving`

#### 4. Explanation

- The `prefix` in a message can be either a `servername` or a client identifier.
- For server-originated messages, only the `servername` is used.
- For client-originated messages, the full or partial client identifier is used.
- The full client identifier (`nickname!username@hostname`) provides the most complete information about the message source.
- Partial client identifiers may be used when full information is not available or necessary.

#### 5. When to Use Each Identifier

##### Server Identifier:
- Used for all messages originating from the server itself
- Examples: Numeric replies, PING messages, server notices

##### Client Identifier:
- Used for messages originating from or relating to specific clients
- Full identifier (`nickname!username@hostname`) should be used whenever possible, especially for:
  - PRIVMSG and NOTICE commands
  - JOIN, PART, and QUIT notifications
  - NICK changes
  - Any other user actions that affect channels or other users
- Partial identifiers might be used in specific contexts or when full information is not available

#### 6. Important Notes

The server should always use the most complete form of the client identifier available when relaying messages from one client to another.


### IRC Commands vs. Numeric Replies

In IRC, there are two types of messages that can be sent:

1. **Commands**: These are text-based instructions sent from clients to servers. Examples include NICK, JOIN, PRIVMSG, etc. These are not represented by numbers.

2. **Numeric Replies**: These are responses sent from servers to clients, often in response to commands. These use three-digit numbers.
  -  Numeric replies that start with "ERR_" are indeed error messages.
     These indicate that something went wrong or that a requested action couldn't be completed.
  -  Numeric replies that start with "RPL_" are standard replies. 
     These are typically used for successful responses or to provide requested information.


The statement from RFC 2812:

```
The command MUST either be a valid IRC command or a three (3) digit
number represented in ASCII text.
```

This means that in the message format:

```
[:<prefix>] <command> <parameters><CR><LF>
```

The `<command>` part can be either:

- A text-based command (when sent from client to server)
- A three-digit number (when sent from server to client as a response)

## Examples:

1. Client to Server:
   ```
   NICK Alice
   ```
   Here, "NICK" is the command.

2. Server to Client:
   ```
   :server.name 001 Alice :Welcome to the Internet Relay Network Alice!Alice@your.host
   ```
   Here, "001" is the three-digit number representing RPL_WELCOME.

It's important to note that clients typically send text-based commands, while servers respond with both text-based messages and numeric replies. The numeric replies are standardized to allow clients to easily parse and understand server responses.
In IRC, there are two types of messages that can be sent:


### Client-Server Messages (RFC 2812)

Quote:
"Servers and clients send each other messages, which may or may not
generate a reply.
If the message contains a valid command,  the client should expect a reply as
specified but it is not advised to wait forever for the reply;
client to server and server to server communication is essentially 
asynchronous by nature.""

### Message Format

IRC messages have the following general format:

```
[:<prefix>] <command> <parameters><CR><LF>
```

- `prefix` (optional): Indicates the origin of the message.
- `command`: A valid IRC command or a three-digit number.
- `parameters`: Up to 15 parameters for the command, space-separated.
- `<CR><LF>`: Carriage Return followed by Line Feed, explained in detail below.

Quote:
"The prefix, command, and all parameters are separated by one ASCII space character (0x20) each.
The presence of a prefix is indicated with a single leading ASCII
colon character (':', 0x3b), which MUST be the first character of the message itself.
There MUST be NO gap (whitespace) between the colon and the prefix.
The prefix is used by servers to indicate the true origin of the message.
If the prefix is missing from the message, it is assumed to have originated from the connection
from which it was received from.
Clients SHOULD NOT use a prefix when sending a message;
if they use one, the only valid prefix is the registered nickname associated with the client."

"IRC messages are always lines of characters terminated with a CR-LF (Carriage Return - Line Feed) pair,
 and these messages SHALL NOT exceed 512 characters in length, counting all characters including
the trailing CR-LF.
Thus, there are 510 characters maximum allowed for the command and its parameters.
There is no provision for continuation of message lines.  See section 6 for more details about
   current implementations."

### CR-LF Explanation

CR-LF stands for "Carriage Return - Line Feed". It's a combination of two control characters:

1. CR (Carriage Return): ASCII code 13, often represented as \r
2. LF (Line Feed): ASCII code 10, often represented as \n

In the context of IRC (and many other network protocols), CR-LF is used to denote the end of a message. This pairing comes from the early days of teleprinters:

- Carriage Return moved the cursor to the beginning of the line.
- Line Feed moved the paper up by one line.

Together, they create a new line, which in modern terms is equivalent to pressing the "Enter" key.

### Example with CR-LF

Here's an example of a PRIVMSG command with CR-LF explicitly shown:

```
PRIVMSG #channel :Hello, world!<CR><LF>
```

In most programming contexts, this would be represented as:

```
"PRIVMSG #channel :Hello, world!\r\n"
```

When sending messages in your IRC server implementation, you must ensure that each message ends with this CR-LF pair.

## Rules for Valid Commands

1. Commands are case-insensitive.
2. The total length of a message must not exceed 512 characters, including CR-LF.
3. There can be up to 15 parameters.
4. The last parameter can be prefixed with a colon `:` to include spaces.




### 1. Connection and Registration

#### PASS - Set Connection Password
- Functionality: Set the connection password before registration
- Status: Implemented
- Repeatability: Can only be used once before registration is complete
- Official Syntax: `PASS <password>`
- Example: `PASS secretpassword`

Numeric Replies(List):
- ERR_NEEDMOREPARAMS (461): Returned when no password is provided
- ERR_ALREADYREGISTRED (462): Returned if PASS is sent after registration is complete
- ERR_PASSWDMISMATCH (464): Returned to indicate a failed attempt at registering a connection for which a password was required and was either not given or incorrect.


Numeric Replies(Full Description):
- ERR_NEEDMOREPARAMS (461):
Type: Error Message
Description: Returned when no password is provided
Client Message: "<command> :Not enough parameters"

- ERR_ALREADYREGISTRED (462):
Type: Error Message
Description: Returned if PASS is sent after registration is complete
Client Message: ":Unauthorized command (already registered)"

- ERR_PASSWDMISMATCH (464):
Type: Error Message
Description: Returned to indicate a failed attempt at registering a connection for which a password was required and was either not given or incorrect.
Client Message: ":Password incorrect"

#### NICK - Set or Change Nickname
- Functionality: Set initial nickname or change current nickname
- Status: Implemented
- Repeatability: Can be used multiple times, both during and after registration
- Official Syntax: `NICK <nickname>`
- Example 1 (Set initial): `NICK Alice`
- Example 2 (Change): `NICK Alice2`


##### Nickname validation

in Section 2.3.

```
nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
special    =  %x5B-60 / %x7B-7D
                  ; "[", "]", "\", "`", "_", "^", "{", "|", "}"
letter     =  %x41-5A / %x61-7A       ; A-Z / a-z
digit      =  %x30-39                 ; 0-9
```

- The nickname definition uses ABNF (Augmented Backus-Naur Form) notation. (*8 means "up to 8")
- The maximum length of a nickname according to RFC 2812 is 9 characters: 1 (the first character) + 8 (the maximum additional characters). 
- The first character must be a letter or one of the special characters defined in the RFC.
- The remaining characters can be letters, digits, hyphens, or the special characters.


Numeric Replies(List):
- ERR_NONICKNAMEGIVEN (431): Returned when no nickname is provided
- ERR_ERRONEUSNICKNAME (432): Returned when the nickname contains invalid characters
- ERR_NICKNAMEINUSE (433): Returned when the chosen nickname is already in use
- ERR_UNAVAILRESOURCE (437): Returned when the nickname is temporarily unavailable (e.g., due to server-side nickname hold)
- ERR_RESTRICTED (484): Returned when the connection is restricted and nickname changes are not allowed



Numeric Replies(Full Description):
- ERR_NONICKNAMEGIVEN (431):
Type: Error Message
Description: Returned when no nickname is provided
Client Message: ":No nickname given"

- ERR_ERRONEUSNICKNAME (432):
Type: Error Message
Description: Returned when the nickname contains invalid characters
Nickname must not contain spaces, commas, asterisks, question marks, exclamation marks, or at signs.
Client Message: "<nick> :Erroneous nickname"

- ERR_NICKNAMEINUSE (433):
Type: Error Message
Description: Returned when the chosen nickname is already in use
Client Message: "<nick> :Nickname is already in use"

- ERR_UNAVAILRESOURCE (437):
Type: Error Message
Description: Returned when the nickname is temporarily unavailable (e.g., due to server-side nickname hold)
Client Message: "<nick/channel> :Nick/channel is temporarily unavailable"

- ERR_RESTRICTED (484):
Type: Error Message
Description: Returned when the connection is restricted and nickname changes are not allowed
Client Message: ":Your connection is restricted!"

#### USER - Set Username and Realname
- Functionality: Set the username and realname during registration
- Status: Implemented
- Repeatability: Can only be used once during the registration process
- Official Syntax: `USER <username> <mode> <unused> :<realname>`
- Example: `USER alice 0 * :Alice Johnson`

- `alice` is the local username
- `0` is a mode (usually ignored by servers)
- `*` is a placeholder (traditionally for the hostname, but often ignored)
- `Alice Johnson` is the user's real name or description

Numeric Replies(List):
- ERR_NEEDMOREPARAMS (461): Returned when not enough parameters are provided
- ERR_ALREADYREGISTRED (462): Returned if USER is sent after registration is complete


- Numeric Replies:
ERR_NEEDMOREPARAMS (461):
Type: Error Message
Description: Returned when not enough parameters are provided
Client Message: "<command> :Not enough parameters"

- ERR_ALREADYREGISTRED (462):
Type: Error Message
Description: Returned if USER is sent after registration is complete
Client Message: ":Unauthorized command (already registered)"

### 2. Channel Operations

#### JOIN - Join a Channel
- Functionality: Join an existing channel or create a new one
Channel names must begin with `&`, `#`, `+`, or `!`.
- Status: Implemented
- Repeatability: Can be used multiple times to join different channels
- Official Syntax: `JOIN <channel>{,<channel>} [<key>{,<key>}]`
- Examples:
  - Join existing: `JOIN #general`
  - Join with key: `JOIN #private secretkey`
  - Join multiple: `JOIN #channel1,#channel2,#channel3`
  - Create new: `JOIN #newchannel`

Numeric Replies(List):
- ERR_NEEDMOREPARAMS (461): Returned when no channel name is provided
- ERR_NOSUCHCHANNEL (403): Returned when the channel name is invalid
- ERR_TOOMANYCHANNELS (405): Returned when the client has joined too many channels
- ERR_BADCHANNELKEY (475): Returned when the channel key (password) is incorrect
- ERR_BANNEDFROMCHAN (474): Returned when the client is banned from the channel
- ERR_INVITEONLYCHAN (473): Returned when the channel is invite-only and the client is not invited
- ERR_CHANNELISFULL (471): Returned when the channel has reached its user limit
- RPL_TOPIC (332): Sent to the client with the channel's topic upon successful join
- RPL_NAMREPLY (353): Sent to the client with the list of users in the channel
- RPL_ENDOFNAMES (366): Marks the end of the NAMES list


Numeric Replies:
- ERR_NEEDMOREPARAMS (461):
Type: Error Message
Description: Returned when no channel name is provided
Client Message: "<command> :Not enough parameters"

- ERR_NOSUCHCHANNEL (403):
Type: Error Message
Description: Returned when the channel name is invalid
Client Message: "<channel name> :No such channel"

- ERR_TOOMANYCHANNELS (405):
Type: Error Message
Description: Returned when the client has joined too many channels
Client Message: "<channel name> :You have joined too many channels"

- ERR_BADCHANNELKEY (475):
Type: Error Message
Description: Returned when the channel key (password) is incorrect
Client Message: "<channel> :Cannot join channel (+k)"

- ERR_BANNEDFROMCHAN (474):
Type: Error Message
Description: Returned when the client is banned from the channel
Client Message: "<channel> :Cannot join channel (+b)"

- ERR_INVITEONLYCHAN (473):
Type: Error Message
Description: Returned when the channel is invite-only and the client is not invited
Client Message: "<channel> :Cannot join channel (+i)"

- ERR_CHANNELISFULL (471):
Type: Error Message
Description: Returned when the channel has reached its user limit
Client Message: "<channel> :Cannot join channel (+l)"

- RPL_TOPIC (332):
Type: Reply Message
Description: Sent to the client with the channel's topic upon successful join
Client Message: "<channel> :<topic>"

- RPL_NAMREPLY (353):
Type: Reply Message
Description: Sent to the client with the list of users in the channel
Client Message: "( "=" / "*" / "@" ) <channel> :[ "@" / "+" ] <nick> *( " " [ "@" / "+" ] <nick> )"

- RPL_ENDOFNAMES (366):
Type: Reply Message
Description: Marks the end of the NAMES list
Client Message: "<channel> :End of NAMES list"

#### PART - Leave a Channel
- Functionality: Leave one or more channels
- Status: Not Implemented
- Repeatability: Can be used multiple times for different channels
- Official Syntax: `PART <channel>{,<channel>} [<reason>]`
- Examples:
  - Leave one: `PART #general`
  - Leave multiple: `PART #channel1,#channel2`
  - Leave with message: `PART #general :Goodbye everyone!`

Numeric Replies (Short List):
- ERR_NEEDMOREPARAMS (461): Returned when no channel name is provided
- ERR_NOSUCHCHANNEL (403): Returned when the specified channel doesn't exist
- ERR_NOTONCHANNEL (442): Returned when the client is not on the specified channel


Numeric Replies(Full Description):
- ERR_NEEDMOREPARAMS (461):
Type: Error Message
Description: Returned when no channel name is provided
Client Message: "<command> :Not enough parameters"

- ERR_NOTONCHANNEL (442):
Type: Error Message
Description: Returned when the client is not on the specified channel
Client Message: "<channel> :You're not on that channel"

- ERR_CHANOPRIVSNEEDED (482):
Type: Error Message
Description: Returned when the client tries to change the topic without having the necessary privileges
Client Message: "<channel> :You're not channel operator"

- RPL_NOTOPIC (331):
Type: Reply Message
Description: Returned when the channel has no topic set
Client Message: "<channel> :No topic is set"

- RPL_TOPIC (332):
Type: Reply Message
Description: Returned with the current topic when querying or after successfully setting a new topic
Client Message: "<channel> :<topic>"

#### TOPIC - View or Change Channel Topic
- Functionality: View the current topic or set a new one
- Status: Implemented
- Repeatability: Can be used multiple times
- Official Syntax: `TOPIC <channel> [<topic>]`
- Examples:
  - View: `TOPIC #general`
  - Set: `TOPIC #general :Welcome to the general discussion channel!`

Numeric Replies(Short List):
- ERR_NEEDMOREPARAMS (461): Returned when no channel name is provided
- ERR_NOTONCHANNEL (442): Returned when the client is not on the specified channel
- ERR_CHANOPRIVSNEEDED (482): Returned when the client tries to change the topic without having the necessary privileges
- RPL_NOTOPIC (331): Returned when the channel has no topic set
- RPL_TOPIC (332): Returned with the current topic when querying or after successfully setting a new topic


Numeric Replies(Full Description):
- ERR_NEEDMOREPARAMS (461):
Type: Error Message
Description: Returned when no channel name is provided
Client Message: "<command> :Not enough parameters"

- ERR_NOTONCHANNEL (442):
Type: Error Message
Description: Returned when the client is not on the specified channel
Client Message: "<channel> :You're not on that channel"

- ERR_CHANOPRIVSNEEDED (482):
Type: Error Message
Description: Returned when the client tries to change the topic without having the necessary privileges
Client Message: "<channel> :You're not channel operator"

- RPL_NOTOPIC (331):
Type: Reply Message
Description: Returned when the channel has no topic set
Client Message: "<channel> :No topic is set"

- RPL_TOPIC (332):
Type: Reply Message
Description: Returned with the current topic when querying or after successfully setting a new topic
Client Message: "<channel> :<topic>"

#### NAMES - List Users in a Channel
- Functionality: List all users in a specified channel
- Status: Not Implemented
- Repeatability: Can be used multiple times
- Official Syntax: `NAMES [<channel>{,<channel>}]`
- Example: `NAMES #general`

Numeric Replies(List):
- RPL_NAMREPLY (353): Returned for each chunk of names in the channel
- RPL_ENDOFNAMES (366): Marks the end of the NAMES list

 Numeric Replies(Full Description):
- RPL_NAMREPLY (353):
Type: Reply Message
Description: Returned for each chunk of names in the channel
Client Message: "( "=" / "*" / "@" ) <channel> :[ "@" / "+" ] <nick> *( " " [ "@" / "+" ] <nick> )"

- RPL_ENDOFNAMES (366):
Type: Reply Message
Description: Marks the end of the NAMES list
Client Message: "<channel> :End of NAMES list"

#### LIST - List Channels
- Functionality: List all visible channels on the server
- Status: Not Implemented
- Repeatability: Can be used multiple times
- Official Syntax: `LIST [<channel>{,<channel>} [<server>]]`
- Examples:
  - List all: `LIST`
  - List specific: `LIST #general,#help`

Numeric Replies(List):
- RPL_LISTSTART (321): Marks the start of the LIST response (optional)
- RPL_LIST (322): Returned for each channel in the list
- RPL_LISTEND (323): Marks the end of the LIST response

Numeric Replies(Full Description):
- RPL_LISTSTART (321):
Type: Reply Message
Description: Marks the start of the LIST response (optional)
Client Message: "Channel :Users  Name"

- RPL_LIST (322):
Type: Reply Message
Description: Returned for each channel in the list
Client Message: "<channel> <# visible> :<topic>"

- RPL_LISTEND (323):
Type: Reply Message
Description: Marks the end of the LIST response
Client Message: ":End of LIST"

### 3. Messaging

#### PRIVMSG - Send Private Message
- Functionality: Send a private message to a user or channel
- Status: Implemented
- Repeatability: Can be used multiple times
- Official Syntax: `PRIVMSG <receiver>{,<receiver>} :<text>`
- Examples:
  - To user: `PRIVMSG Bob :Hey, how are you?`
  - To channel: `PRIVMSG #general :Hello everyone!`

Numeric Replies(List):
- ERR_NORECIPIENT (411): Returned when no recipient is given
- ERR_NOTEXTTOSEND (412): Returned when no text to send is given
- ERR_CANNOTSENDTOCHAN (404): Returned when the client cannot send to the channel (e.g., moderated channel)
- ERR_NOTOPLEVEL (413): Returned when no toplevel domain specified
- ERR_WILDTOPLEVEL (414): Returned when wildcard in toplevel domain
- ERR_TOOMANYTARGETS (407): Returned when too many targets are specified
- ERR_NOSUCHNICK (401): Returned when the nickname or channel name doesn't exist

Numeric Replies(Full Description):
- ERR_NORECIPIENT (411):
Type: Error Message
Description: Returned when no recipient is given
Client Message: ":No recipient given (<command>)"

- ERR_NOTEXTTOSEND (412):
Type: Error Message
Description: Returned when no text to send is given
Client Message: ":No text to send"

- ERR_CANNOTSENDTOCHAN (404):
Type: Error Message
Description: Returned when the client cannot send to the channel (e.g., moderated channel)
Client Message: "<channel name> :Cannot send to channel"

- ERR_NOTOPLEVEL (413):
Type: Error Message
Description: Returned when no toplevel domain specified
Client Message: "<mask> :No toplevel domain specified"

- ERR_WILDTOPLEVEL (414):
Type: Error Message
Description: Returned when wildcard in toplevel domain
Client Message: "<mask> :Wildcard in toplevel domain"

- ERR_TOOMANYTARGETS (407):
Type: Error Message
Description: Returned when too many targets are specified
Client Message: "<target> :<error code> recipients. <abort message>"

- ERR_NOSUCHNICK (401):
Type: Error Message
Description: Returned when the nickname or channel name doesn't exist
Client Message: "<nickname> :No such nick/channel"

#### NOTICE - Send Notice
- Functionality: Send a notice to a user or channel (similar to PRIVMSG but should not trigger automatic replies)
- Status: Not Implemented
- Repeatability: Can be used multiple times
- Official Syntax: `NOTICE <nickname> :<text>`
- Examples:
  - To user: `NOTICE Bob :Important: Server will restart in 5 minutes`
  - To channel: `NOTICE #general :Channel rules updated. Please check the topic.`

Numeric Replies:
- None. NOTICE does not generate any error replies to avoid loops.

### 4. User Information

#### WHOIS - Query User Information
- Functionality: Request information about a specific user
- Status: Not Implemented
- Repeatability: Can be used multiple times
- Official Syntax: `WHOIS [<server>] <nickmask>[,<nickmask>[,...]]`
- Example: `WHOIS Alice`

Numeric Replies(List):
- RPL_WHOISUSER (311): Returns the user's nickname, username, hostname, and real name
- RPL_WHOISCHANNELS (319): Lists the channels the user is on
- RPL_WHOISSERVER (312): Shows which server the user is connected to
- RPL_AWAY (301): Sent if the user is away
- RPL_WHOISOPERATOR (313): Sent if the user is an IRC operator
- RPL_WHOISIDLE (317): Shows how long the user has been idle
- RPL_ENDOFWHOIS (318): Marks the end of the WHOIS reply
- ERR_NOSUCHNICK (401): Returned when the specified nickname is not found

Numeric Replies(Full Description):
- RPL_WHOISUSER (311):
Type: Reply Message
Description: Returns the user's nickname, username, hostname, and real name
Client Message: "<nick> <user> <host> * :<real name>"

- RPL_WHOISCHANNELS (319):
Type: Reply Message
Description: Lists the channels the user is on
Client Message: "<nick> :*( ( "@" / "+" ) <channel> " " )"

- RPL_WHOISSERVER (312):
Type: Reply Message
Description: Shows which server the user is connected to
Client Message: "<nick> <server> :<server info>"

- RPL_AWAY (301):
Type: Reply Message
Description: Sent if the user is away
Client Message: "<nick> :<away message>"

- RPL_WHOISOPERATOR (313):
Type: Reply Message
Description: Sent if the user is an IRC operator
Client Message: "<nick> :is an IRC operator"

- RPL_WHOISIDLE (317):
Type: Reply Message
Description: Shows how long the user has been idle
Client Message: "<nick> <integer> :seconds idle"

- RPL_ENDOFWHOIS (318):
Type: Reply Message
Description: Marks the end of the WHOIS reply
Client Message: "<nick> :End of WHOIS list"

- ERR_NOSUCHNICK (401):
Type: Error Message
Description: Returned when the specified nickname is not found
Client Message: "<nickname> :No such nick/channel"

#### WHO - Query Channel or User Information
- Functionality: Request a list of users matching a given mask
- Status: Not Implemented
- Repeatability: Can be used multiple times
- Official Syntax: `WHO [<mask> [o]]`
- Examples:
  - Channel: `WHO #general`
  - User mask: `WHO Alice*`

Numeric Replies(List):
- RPL_WHOREPLY (352): Returned for each user matching the WHO query
- RPL_ENDOFWHO (315): Marks the end of the WHO reply
- ERR_NOSUCHSERVER (402): Returned when the server name is invalid

Numeric Replies(Full Description):
- RPL_WHOREPLY (352):
Type: Reply Message
Description: Returned for each user matching the WHO query
Client Message: "<channel> <user> <host> <server> <nick> ( "H" / "G" > ["*"] [ ( "@" / "+" ) ] :<hopcount> <real name>"

- RPL_ENDOFWHO (315):
Type: Reply Message
Description: Marks the end of the WHO reply
Client Message: "<name> :End of WHO list"

- ERR_NOSUCHSERVER (402):
Type: Error Message
Description: Returned when the server name is invalid
Client Message: "<server name> :No such server"

### 5. Miscellaneous

#### PING - Test Connection
- Functionality: Test if the server is still responsive
- Status: Not Implemented
- Repeatability: Can be used multiple times
- Official Syntax: `PING <server1> [<server2>]`
- Example: `PING :test123`

Numeric Replies(List):
Type: Error Message
- ERR_NOORIGIN (409): Returned when no origin specified
- ERR_NOSUCHSERVER (402): Returned when the server name is invalid

Numeric Replies(Full Description):
- ERR_NOORIGIN (409):
Type: Error Message
Description: Returned when no origin specified
Client Message: ":No origin specified"

- ERR_NOSUCHSERVER (402):
Type: Error Message
Description: Returned when the server name is invalid
Client Message: "<server name> :No such server"



#### QUIT - Disconnect from Server
- Functionality: Disconnect from the IRC server
- Status: Not Implemented
- Repeatability: Typically used once per session, as it terminates the connection
- Official Syntax: `QUIT [<quit message>]`
- Examples:
  - Simple: `QUIT`
  - With message: `QUIT :Goodbye, see you tomorrow!`

Numeric Replies:
- None. The server acknowledges QUIT by closing the connection.

## Operator Actions

### 1. Channel Management

#### MODE - Set Channel or User Modes
- Functionality: Change channel or user modes
- Status: Implemented
- Repeatability: Can be used multiple times
- Official Syntax for channel modes: `MODE <channel> <modestring> [<mode arguments>...]`
- Official Syntax for user modes: `MODE <nickname> <modestring>`
 Parameters: <channel> *( ( "-" / "+" ) *<modes> *<modeparams> )
 RFC 2812, section 3.2.3: The maximum number of parameter modes is three (3) per command.
- Examples:
  - Set invite-only: `MODE #channel +i`
  - Remove invite-only: `MODE #channel -i`
  - Set channel key: `MODE #channel +k secretkey`
  - Remove channel key: `MODE #channel -k secretkey`
  - Give channel operator status: `MODE #channel +o Alice`
  - Take away channel operator status: `MODE #channel -o Alice`
  - Set user limit: `MODE #channel +l 50`
  - Remove user limit: `MODE #channel -l`
  - Set user mode (invisible): `MODE Alice +i`
  - Unset user mode (invisible): `MODE Alice -i`

Supported Channel Modes:
- `+i/-i`: Set/remove Invite-only channel
- `+t/-t`: Set/remove the restrictions of the TOPIC command to channel operators
- `+k/-k`: Set/remove the channel key (password)
- `+o/-o`: Give/take channel operator privilege
- `+l/-l`: Set/remove the user limit to channel

Supported User Modes:
- `+i/-i`: Set/unset invisible mode
- `+w/-w`: Set/unset receive wallops


Numeric Replies(List):
- ERR_NEEDMOREPARAMS (461): Returned when not enough parameters are given
- ERR_CHANOPRIVSNEEDED (482): Returned when the client doesn't have the necessary privileges
- ERR_NOSUCHNICK (401): Returned when the given nickname is not found
- ERR_USERSDONTMATCH (502): Returned when a user tries to view or change the modes for another user
- ERR_UMODEUNKNOWNFLAG (501): Returned when an unknown MODE flag is given
- RPL_CHANNELMODEIS (324): Returned when querying channel modes
- RPL_UMODEIS (221): Returned when querying user modes



Numeric Replies(Full Description):
- ERR_NEEDMOREPARAMS (461):
Type: Error Message
Description: Returned when not enough parameters are given
Client Message: "<command> :Not enough parameters"

- ERR_CHANOPRIVSNEEDED (482):
Type: Error Message
Description: Returned when the client doesn't have the necessary privileges
Client Message: "<channel> :You're not channel operator"

- ERR_NOSUCHNICK (401):
Type: Error Message
Description: Returned when the given nickname is not found
Client Message: "<nickname> :No such nick/channel"

- ERR_USERSDONTMATCH (502):
Type: Error Message
Description: Returned when a user tries to view or change the modes for another user
Client Message: ":Cannot change mode for other users"

- ERR_UMODEUNKNOWNFLAG (501):
Type: Error Message
Description: Returned when an unknown MODE flag is given
Client Message: ":Unknown MODE flag"

- RPL_CHANNELMODEIS (324):
Type: Reply Message
Description: Returned when querying channel modes
Client Message: "<channel> <mode> <mode params>"

- RPL_UMODEIS (221):
Type: Reply Message
Description: Returned when querying user modes
Client Message: "<user mode string>"

#### KICK - Remove User from Channel
- Functionality: Forcibly remove a user from a channel
- Status: Implemented
- Repeatability: Can be used multiple times
- Official Syntax: `KICK <channel> <user> [<comment>]`
- Examples:
  - Simple: `KICK #general Alice`
  - With reason: `KICK #general Alice :Violation of channel rules`

Numeric Replies(List):
- ERR_NEEDMOREPARAMS (461): Returned when not enough parameters are given
- ERR_NOSUCHCHANNEL (403): Returned when the given channel doesn't exist
- ERR_BADCHANMASK (476): Returned when the channel name is invalid
- ERR_CHANOPRIVSNEEDED (482): Returned when the client doesn't have the necessary privileges
- ERR_USERNOTINCHANNEL (441): Returned when the user is not in the specified channel
- ERR_NOTONCHANNEL (442): Returned when the client is not on the specified channel


Numeric Replies(Full Description):
- ERR_NEEDMOREPARAMS (461):
Type: Error Message
Description: Returned when not enough parameters are given
Client Message: "<command> :Not enough parameters"

- ERR_NOSUCHCHANNEL (403):
Type: Error Message
Description: Returned when the given channel doesn't exist
Client Message: "<channel name> :No such channel"

- ERR_BADCHANMASK (476):
Type: Error Message
Description: Returned when the channel name is invalid
Client Message: "<channel> :Bad Channel Mask"

- ERR_CHANOPRIVSNEEDED (482):
Type: Error Message
Description: Returned when the client doesn't have the necessary privileges
Client Message: "<channel> :You're not channel operator"

- ERR_USERNOTINCHANNEL (441):
Type: Error Message
Description: Returned when the user is not in the specified channel
Client Message: "<nick> <channel> :They aren't on that channel"

- ERR_NOTONCHANNEL (442):
Type: Error Message
Description: Returned when the client is not on the specified channel
Client Message: "<channel> :You're not on that channel"


#### INVITE - Invite User to Channel
- Functionality: Invite a user to join a channel
- Status: Implemented
- Repeatability: Can be used multiple times
- Official Syntax: `INVITE <nickname> <channel>`
- Example: `INVITE Bob #private`

Numeric Replies(List):
- ERR_NEEDMOREPARAMS (461): Returned when not enough parameters are given
- ERR_NOSUCHNICK (401): Returned when the given nickname is not found
- ERR_NOTONCHANNEL (442): Returned when the client is not on the specified channel
- ERR_USERONCHANNEL (443): Returned when the invited user is already on the channel
- ERR_CHANOPRIVSNEEDED (482): Returned when the client doesn't have the necessary privileges
- RPL_INVITING (341): Sent to the inviting client to confirm the invitation
- RPL_AWAY (301): Sent if the invited user is away

Numeric Replies(Full Description):
- ERR_NEEDMOREPARAMS (461):
Type: Error Message
Description: Returned when not enough parameters are given
Client Message: "<command> :Not enough parameters"

- ERR_NOSUCHNICK (401):
Type: Error Message
Description: Returned when the given nickname is not found
Client Message: "<nickname> :No such nick/channel"

- ERR_NOTONCHANNEL (442):
Type: Error Message
Description: Returned when the client is not on the specified channel
Client Message: "<channel> :You're not on that channel"

- ERR_USERONCHANNEL (443):
Type: Error Message
Description: Returned when the invited user is already on the channel
Client Message: "<user> <channel> :is already on channel"

- ERR_CHANOPRIVSNEEDED (482):
Type: Error Message
Description: Returned when the client doesn't have the necessary privileges
Client Message: "<channel> :You're not channel operator"

- RPL_INVITING (341):
Type: Reply Message
Description: Sent to the inviting client to confirm the invitation
Client Message: "<channel> <nick>"

- RPL_AWAY (301):
Type: Reply Message
Description: Sent if the invited user is away
Client Message: "<nick> :<away message>"

### 2. Server Management Commands (Not Implemented)

The following commands are typically available to IRC operators but are not implemented in our current server:

## KILL - Disconnect User from Server
- Functionality: Forcibly disconnect a user from the server
- Status: Not Implemented
- Official Syntax: `KILL <nickname> <comment>`
- Example: `KILL Alice :Violation of server rules`

Numeric Replies(List):
- ERR_NOPRIVILEGES (481): Returned if a non-operator tries to use this command
- ERR_NEEDMOREPARAMS (461): Returned when not enough parameters are given
- ERR_NOSUCHNICK (401): Returned when the given nickname is not found
- ERR_CANTKILLSERVER (483): Returned if someone tries to KILL a server


Numeric Replies(Full Description):
- ERR_NOPRIVILEGES (481):
Type: Error Message
Description: Returned if a non-operator tries to use this command
Client Message: ":Permission Denied- You're not an IRC operator"

- ERR_NEEDMOREPARAMS (461):
Type: Error Message
Description: Returned when not enough parameters are given
Client Message: "<command> :Not enough parameters"

- ERR_NOSUCHNICK (401):
Type: Error Message
Description: Returned when the given nickname is not found
Client Message: "<nickname> :No such nick/channel"

- ERR_CANTKILLSERVER (483):
Type: Error Message
Description: Returned if someone tries to KILL a server
Client Message: ":You can't kill a server!"

## REHASH - Reload Server Configuration
- Functionality: Reload the server configuration file
- Status: Not Implemented
- Official Syntax: `REHASH`
- Example: `REHASH`

Numeric Replies(List):
- ERR_NOPRIVILEGES (481): Returned if a non-operator tries to use this command
- RPL_REHASHING (382): Sent to confirm that the config file is being rehashed

Numeric Replies(Full Description):
- ERR_NOPRIVILEGES (481):
Type: Error Message
Description: Returned if a non-operator tries to use this command
Client Message: ":Permission Denied- You're not an IRC operator"

- RPL_REHASHING (382):
Type: Reply MessageType: Reply Message
Description: Sent to confirm that the config file is being rehashed
Client Message: "<config file> :Rehashing"

## RESTART - Restart the Server
- Functionality: Restart the IRC server
- Status: Not Implemented
- Official Syntax: `RESTART`
- Example: `RESTART`

Numeric Replies(List):
- ERR_NOPRIVILEGES (481): Returned if a non-operator tries to use this command

Numeric Replies(Full Description):
- ERR_NOPRIVILEGES (481):
Type: Error Message
Description: Returned if a non-operator tries to use this command
Client Message: ":Permission Denied- You're not an IRC operator"

## DIE - Shut Down the Server
- Functionality: Shut down the IRC server
- Status: Not Implemented
- Official Syntax: `DIE`
- Example: `DIE`

Numeric Replies(List):
- ERR_NOPRIVILEGES (481): Returned if a non-operator tries to use this command

Numeric Replies(Full Description):
- ERR_NOPRIVILEGES (481):
Type: Error Message
Description: Returned if a non-operator tries to use this command
Client Message: ":Permission Denied- You're not an IRC operator"

Note: These commands are powerful and can significantly impact server operations. In a production environment, their use should be strictly controlled and logged.












----





### IRC Message Formats: Nicknames vs Full Identifiers in RFC 2812

##### 1. Server-to-Client Messages (Numeric Replies)

According to RFC 2812, server-to-client messages, including error responses and numeric replies, should use the client's nickname, not the full identifier.

##### Reference:
Section 2.4 of RFC 2812 states:

```
The numeric reply MUST be sent as one message consisting of the sender
prefix (servername), the three-digit numeric, and the target of the
reply.
```

Further, in the same section:

```
The target of the numeric reply is usually a client's nickname.
```

##### Format:
`:server_name numeric nickname :message`

##### Example:
`:irc.example.com 001 Alice :Welcome to the Internet Relay Network`

#### 2. Client-to-Client Messages (Relayed by Server)

For messages that originate from a client and are relayed to other clients by the server (such as JOIN, PART, PRIVMSG to a channel, etc.), the full client identifier should be used.

##### Reference:
Section 2.3.1 of RFC 2812 defines the message format:


#### Conclusion

1. For server-generated messages (like numeric replies), use only the nickname of the target client.
2. For client-originated messages that are relayed to other clients, use the full client identifier (nickname!username@hostname).

This distinction ensures compliance with RFC 2812 and provides the appropriate level of information for each type of message in the IRC protocol.



### IRC Server and Client Identifiers

#### 1. RFC 2812 Section 2.3.1 Excerpt

The following is an excerpt from RFC 2812, Section 2.3.1, which defines the message format in Augmented BNF:

```
prefix     =  servername / ( nickname [ [ "!" user ] "@" host ] )
```

#### 2. Server Identifier

- Format: `servername`
- Example: `:irc.example.com`

The server identifier is simply the name of the server. It's used as the prefix in messages originating from the server, such as numeric replies or server-to-client messages like PING.

Usage examples:
1. Numeric reply: `:irc.example.com 001 nickname :Welcome to the IRC Network`
2. PING message: `:irc.example.com PING :irc.example.com`

#### 3. Client Identifier

- Full Format: `nickname!username@hostname`
- Partial Formats: 
  - `nickname`
  - `nickname@hostname`
  - `nickname!username`

The client identifier can appear in full or partial forms, depending on the context and available information.

Usage examples:
1. Full identifier: `:john!jdoe@example.com PRIVMSG #channel :Hello, world!`
2. Partial (nickname only): `:john QUIT :Leaving`

#### 4. Explanation

- The `prefix` in a message can be either a `servername` or a client identifier.
- For server-originated messages, only the `servername` is used.
- For client-originated messages, the full or partial client identifier is used.
- The full client identifier (`nickname!username@hostname`) provides the most complete information about the message source.
- Partial client identifiers may be used when full information is not available or necessary.

#### 5. When to Use Each Identifier

##### Server Identifier:
- Used for all messages originating from the server itself
- Examples: Numeric replies, PING messages, server notices

##### Client Identifier:
- Used for messages originating from or relating to specific clients
- Full identifier (`nickname!username@hostname`) should be used whenever possible, especially for:
  - PRIVMSG and NOTICE commands
  - JOIN, PART, and QUIT notifications
  - NICK changes
  - Any other user actions that affect channels or other users
- Partial identifiers might be used in specific contexts or when full information is not available

#### 6. Important Notes

The server should always use the most complete form of the client identifier available when relaying messages from one client to another.
