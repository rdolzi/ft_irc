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



/*
SCENARIO 1: Registration Phase (only one client)

Client 1:
1)- Test pass is the first thing to do
- Expected: ERROR
- NICK test


2) PASS 
- Expected: ERR_NEEDMOREPARAMS (461)

3) PASS wrongpsw
- Expected: ERR_PASSWDMISMATCH (464)

4) PASS psw
- Expected: Password correctly set

5) PASS psw
- Expected: ERR_ALREADYREGISTRED (462)


6) NICK
- Expected:

7) NICK this@
- Expected: ERR_ERRONEUSNICKNAME

8) NICK firstNikc
- Expected: Set correctly nick

9) NICK secondNick
- Expected: Change correctly nick


10) USER 
- Expected: ERR_NEEDMOREPARAMS (461)

11) USER testUser 0 * :real Name
- Expected: Registration Done. Welcome message.

12)  USER testUser 0 * :real Name
- Expected:  ERR_ALREADYREGISTRED (462)

13) Client Disconnect

---
SCENARIO 2: Interactive phase. Create and join channels, testing operator and base roles, testing messaging (Two clients).

- Both client register succesfully
1) Client 1:
 PASS psw
 NICK client1
 USER first 0 * :First Client

2) Client 2:
 PASS psw
 NICK client2
 USER first 0 * :Second Client

3) Client1 will become an operator, client2 remains at base role

-- All action performed by Client1
4) JOIN
- Expected: ERR_NEEDMOREPARAMS (461)

5) JOIN wrong
- Expected: ERR_NOSUCHCHANNEL (403)

6) JOIN #ok
- Expected: Create and join channel as operator user + RPL_TOPIC (332) + RPL_NAMREPLY (353) +RPL_ENDOFNAMES (366)



7) TOPIC
- Expected: ERR_NEEDMOREPARAMS (461)

8) TOPIC #ok
- Expected: RPL_NOTOPIC (331) 

9) TOPIC #ok :new channel topic!
- Expected: set new channel topic > RPL_TOPIC (332)

10) MODE
- Expected: ERR_NEEDMOREPARAMS (461)

11) MODE #ok +g
- Expected: ERR_UMODEUNKNOWNFLAG (501)



12) MODE #channel +i
- Expected:  Set invite-only

-- Now client2 try to join channel in inviteOnly mode, and try operator commands
13) JOIN #ok
- Expected: ERR_INVITEONLYCHAN (473)

14) TOPIC #ok
- Expected: ERR_NOTONCHANNEL (442)

--Now client1 invite client2 and then client2 try operators commands 
--client1
15) INVITE wrong
- Expected: ERR_NOSUCHNICK (401)

16) INVITE client1 #ok
- Expected: RPL_INVITING (341) 

--client2
17) MODE #ok -i
- Expected: ERR_CHANOPRIVSNEEDED (482)

18) TOPIC #ok
- Expected: ERR_CHANOPRIVSNEEDED (482)

19) INVITE client3 #ok
- Expected: ERR_CHANOPRIVSNEEDED (482)

20) KICK #ok client1
- Expected: ERR_CHANOPRIVSNEEDED (482)

21) PRVMSG :manca user
- Expected: ERR_NORECIPIENT (411)

22) PRVIMSG client1 
- Expected: ERR_NOTEXTTOSEND (412)

23) PRIVMSG wrongNick :ciao! 
- Expected: ERR_NOSUCHNICK (401)

24) PRIVMSG client1 :ciao client1!
- Expected: send message to client1

25) PRIVMSG #ok :hello channel!
- Expected: send message to #ok channel

--now client 1
26) KICK #ok client2 :ciaociao!
- Expected: client1 kicks client2 with reason + all members get message of the kick action


*/




/*
check cmd, skipped

NICK
- ERR_NICKNAMEINUSE (433): Returned when the chosen nickname is already in use


JOIN
- ERR_TOOMANYCHANNELS (405): Returned when the client has joined too many channels

TOPIC:
- ERR_CHANOPRIVSNEEDED (482): Returned when the client tries to change the topic without having the necessary privileges
- ERR_NOTONCHANNEL (442): Returned when the client is not on the specified channel
*/


/*
TODO:
- nick max lenght
- canonical form

-privmsg multiclient?
*/