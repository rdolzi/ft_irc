# IRC Protocol Compliance Report

This report details the compliance status of each command with respect to the IRC protocol specifications as outlined in the comprehensive-irc-user-guide.md. Additionally, it explicitly covers a subset of the required commands that are necessary to implement according to the [requirements](./docs/requirements/command-progress-status.md).




## General Protocol Compliance

1. Message formatting:
   - [ ] Messages end with `\r\n` (Not consistently implemented)
   - [ ] Maximum message length of 512 characters including `\r\n` (Not implemented)
   - [ ] Proper handling of the maximum 15 parameters per command (Not implemented)

   Example:
   ```
   PRIVMSG #channel :This is a very long message that exceeds 512 characters...
   ```


2. [x] Case-insensitive command parsing (Implemented in CommandParser::extractCommand)

   Example:
   ```
   privMSG #channel :Hello, World!
   ```

3. Robust data handling:
   - [ ] Proper buffering of incoming data until a complete command is received
   - [ ] Proper handling of partial data reception and packet aggregation
   - [ ] Proper parsing of commands split across multiple TCP packets

   Example (using netcat):
   ```
   $ nc 127.0.0.1 6667
   PRI
   (wait a moment)
   VMSG #
   (wait a moment)
   channel :Hello
   (wait a moment)
   , World!\r\n
   ```


4. Network condition handling:
   - [ ] Proper error handling for various network conditions (including low bandwidth)
   - [ ] Handling of simultaneous commands from multiple clients

   Example (simulating network lag by multiple users):
   ```
   $ nc 127.0.0.1 6667 
   NICK user1\r\n
   (wait 5 seconds)
   USER user1 0 * :Real Name\r\n
   (wait 10 seconds)
   JOIN #channel\r\n
   ```

5. Buffer management:
   - [ ] Proper handling of buffer overflow situations

   Example (sending a very large message):
   ```
   PRIVMSG #channel :Lorem ipsum dolor sit amet... (repeated to exceed buffer size)
   ```




## Command-Specific Compliance

### PASS
- [x] Correct syntax: `PASS <password>`
- [x] Must be sent before any attempt to register the connection
- [x] ERR_NEEDMOREPARAMS (461) if not enough parameters
- [x] ERR_ALREADYREGISTRED (462) if already registered
- [x] ERR_PASSWDMISMATCH (464) failed attempt at registering a connection for which a password was required and was either not given or incorrect.

### NICK
- [x] Correct syntax: `NICK <nickname>`
- [x] Nickname validation (letters, digits, special characters)
- [x] Broadcast nick when updated
- [x] ERR_NONICKNAMEGIVEN (431) if no nickname supplied
- [x] ERR_ERRONEUSNICKNAME (432) if invalid nickname
- [x] ERR_NICKNAMEINUSE (433) if nickname already in use



### USER
- [x] Correct syntax: `USER <username> <mode> <unused> :<realname>`
- [x] ERR_NEEDMOREPARAMS (461) if not enough parameters
- [x] ERR_ALREADYREGISTRED (462) if already registered

### JOIN
- [x] Correct syntax: `JOIN <channel>{,<channel>} [<key>{,<key>}]`
- [x] Support for multiple channels and keys
- [x] Channel name validation (must start with #, &, +, or !)
- [x] ERR_NEEDMOREPARAMS (461) if no channel specified
- [x] ERR_NOSUCHCHANNEL (403) if channel doesn't exist and cannot be created
- [ ] ERR_TOOMANYCHANNELS (405) if user is on too many channels
- [x] ERR_BADCHANNELKEY (475) if channel key (password) is invalid
- [x] ERR_INVITEONLYCHAN (473) if channel is invite-only and user isn't invited
- [x] ERR_CHANNELISFULL (471) if channel is at capacity
- [x] RPL_TOPIC (332) to send channel topic on successful join
- [x] RPL_NAMREPLY (353) to list users in channel after join
- [x] RPL_ENDOFNAMES (366) to end the names list

### TOPIC
- [x] Correct syntax: `TOPIC <channel> [<topic>]`
- [x] Support for setting, clearing, and querying channel topic
- [x] ERR_NEEDMOREPARAMS (461) if no channel specified
- [x] ERR_NOTONCHANNEL (442) if user is not on that channel
- [x] ERR_CHANOPRIVSNEEDED (482) if channel mode +t is set and user is not a channel operator
- [x] RPL_NOTOPIC (331) if no topic is set
- [x] RPL_TOPIC (332) to send the channel topic


### PRIVMSG
- [x] Correct syntax: `PRIVMSG <msgtarget> :<message>`
- [ ] Support for sending to channels and users
- [x] ERR_NORECIPIENT (411) if no recipient given
- [ ] ERR_NOTEXTTOSEND (412) if no message given
- [ ] ERR_CANNOTSENDTOCHAN (404) if cannot send to channel
- [x] ERR_NOSUCHNICK (401) if recipient doesn't exist

### MODE (for channels)
- [x] Correct syntax: `MODE <channel> <modestring> [<mode arguments>...]`
- [x] Support for channel modes: i, t, k, o, l
- [x] ERR_NEEDMOREPARAMS (461) if not enough parameters
- [x] ERR_CHANOPRIVSNEEDED (482) if user doesn't have channel operator privileges
- [x] ERR_UMODEUNKNOWNFLAG (501) if mode character is unknown


### INVITE
- [x] Correct syntax: `INVITE <nickname> <channel>`
- [x] ERR_NEEDMOREPARAMS (461) if not enough parameters
- [x] ERR_NOSUCHNICK (401) if the nickname doesn't exist
- [x] ERR_NOTONCHANNEL (442) if user is not on that channel
- [x] ERR_USERONCHANNEL (443) if user is already on that channel
- [x] ERR_CHANOPRIVSNEEDED (482) if user doesn't have channel operator privileges and channel is invite-only
- [x] RPL_INVITING (341) to confirm invitation

### KICK
- [ ] Correct syntax: `KICK <channel> <user> [<comment>]`
- [ ] ERR_NEEDMOREPARAMS (461) if not enough parameters
- [ ] ERR_NOSUCHCHANNEL (403) if the channel doesn't exist
- [ ] ERR_BADCHANMASK (476) if the channel name is invalid
- [ ] ERR_CHANOPRIVSNEEDED (482) if user doesn't have channel operator privileges
- [ ] ERR_USERNOTINCHANNEL (441) if the user to be kicked is not in the channel
- [ ] ERR_NOTONCHANNEL (442) if the kicking user is not in the channel





