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
- [ ] Must be sent before any attempt to register the connection
- [x] ERR_NEEDMOREPARAMS (461) if not enough parameters
- [x] ERR_ALREADYREGISTRED (462) if already registered

### NICK
- [x] Correct syntax: `NICK <nickname>`
- [ ] Nickname validation (letters, digits, special characters)
- [x] ERR_NONICKNAMEGIVEN (431) if no nickname supplied
- [x] ERR_ERRONEUSNICKNAME (432) if invalid nickname
- [x] ERR_NICKNAMEINUSE (433) if nickname already in use
- [ ] ERR_NICKCOLLISION (436) (Not applicable for single server)

### USER
- [x] Correct syntax: `USER <username> <mode> <unused> :<realname>`
- [ ] Proper handling of mode parameter (currently ignored)
- [ ] Proper handling of unused parameter (should be "*")
- [x] ERR_NEEDMOREPARAMS (461) if not enough parameters
- [x] ERR_ALREADYREGISTRED (462) if already registered

### JOIN
- [ ] Correct syntax: `JOIN <channel>{,<channel>} [<key>{,<key>}]`
- [ ] Support for multiple channels and keys
- [ ] Channel name validation (must start with #, &, +, or !)
- [ ] ERR_NEEDMOREPARAMS (461) if no channel specified
- [ ] ERR_NOSUCHCHANNEL (403) if channel doesn't exist and cannot be created
- [ ] ERR_TOOMANYCHANNELS (405) if user is on too many channels
- [ ] ERR_BADCHANNELKEY (475) if channel key (password) is invalid
- [ ] ERR_BANNEDFROMCHAN (474) if user is banned from the channel
- [ ] ERR_INVITEONLYCHAN (473) if channel is invite-only and user isn't invited
- [ ] ERR_CHANNELISFULL (471) if channel is at capacity
- [ ] RPL_TOPIC (332) to send channel topic on successful join
- [ ] RPL_NAMREPLY (353) to list users in channel after join
- [ ] RPL_ENDOFNAMES (366) to end the names list

### TOPIC
- [ ] Correct syntax: `TOPIC <channel> [<topic>]`
- [ ] Support for setting, clearing, and querying channel topic
- [ ] ERR_NEEDMOREPARAMS (461) if no channel specified
- [ ] ERR_NOTONCHANNEL (442) if user is not on that channel
- [ ] ERR_CHANOPRIVSNEEDED (482) if channel mode +t is set and user is not a channel operator
- [ ] RPL_NOTOPIC (331) if no topic is set
- [ ] RPL_TOPIC (332) to send the channel topic

### PRIVMSG
- [x] Correct syntax: `PRIVMSG <msgtarget> :<message>`
- [ ] Support for sending to channels and users
- [x] ERR_NORECIPIENT (411) if no recipient given
- [ ] ERR_NOTEXTTOSEND (412) if no message given
- [ ] ERR_CANNOTSENDTOCHAN (404) if cannot send to channel
- [x] ERR_NOSUCHNICK (401) if recipient doesn't exist
- [ ] RPL_AWAY (301) if recipient is away

### MODE (for channels)
- [ ] Correct syntax: `MODE <channel> <modestring> [<mode arguments>...]`
- [ ] Support for channel modes: i, t, k, o, l
- [ ] ERR_NEEDMOREPARAMS (461) if not enough parameters
- [ ] ERR_CHANOPRIVSNEEDED (482) if user doesn't have channel operator privileges
- [ ] ERR_NOCHANMODES (477) if channel doesn't support modes
- [ ] ERR_UNKNOWNMODE (472) if mode character is unknown


### INVITE
- [ ] Correct syntax: `INVITE <nickname> <channel>`
- [ ] ERR_NEEDMOREPARAMS (461) if not enough parameters
- [ ] ERR_NOSUCHNICK (401) if the nickname doesn't exist
- [ ] ERR_NOTONCHANNEL (442) if user is not on that channel
- [ ] ERR_USERONCHANNEL (443) if user is already on that channel
- [ ] ERR_CHANOPRIVSNEEDED (482) if user doesn't have channel operator privileges and channel is invite-only
- [ ] RPL_INVITING (341) to confirm invitation

### KICK
- [ ] Correct syntax: `KICK <channel> <user> [<comment>]`
- [ ] ERR_NEEDMOREPARAMS (461) if not enough parameters
- [ ] ERR_NOSUCHCHANNEL (403) if the channel doesn't exist
- [ ] ERR_BADCHANMASK (476) if the channel name is invalid
- [ ] ERR_CHANOPRIVSNEEDED (482) if user doesn't have channel operator privileges
- [ ] ERR_USERNOTINCHANNEL (441) if the user to be kicked is not in the channel
- [ ] ERR_NOTONCHANNEL (442) if the kicking user is not in the channel





