#ifndef BOT_CLASS_HPP
#define BOT_CLASS_HPP

#include <SFML/Network.hpp>

#include "serverMessageStruct.hpp"
#include "ircMimic.hpp"


// TODO (dyrand#4#12/15/2014): fun command, detects if there is fun

class ircBotClass
{
    public:
    ircBotClass();

    //Establishing Connections
    int connectToServer(std::string server, int port);
    int connectToServer();
    int connectionRegistration();

    //Outputs text depending on what staus is
    int statusSwitch(int status);

    //Send and Receive
    void send(std::string formated_text);
    void receive();

    //Basic IRC commands
    void privmsg(std::string channel, std::string message);
    void join(std::string target_channel);
    void join();

    void parserOfServerMessages();
    void postHandler();
    void parserOfMessages();
    void checkCommands();
    void serverResponse();
    void resetVars();
    void loop();

    /*Simple Commands*/ //most likely can remain in ircBotClass
    void disconnect();
    void rawInput();

    /*Object for commands*/
    ircMimic mimic_o;


    serverMessageStruct s_mes_struct; //server_message_struct
    messageStruct         mes_struct; //message_struct

    /*Get functions*/
    std::string getNickname();

    private:

    bool stay_connected;


    long unsigned int bytes_received;
    std::array<char,512> receive_text;
    std::string receive_string;

    std::vector<std::string> block_of_text; //rename
    std::vector<std::string> parsable_strings;

    std::string partial_string;
    std::string temp_string;

    bool partial_flag = false;
    bool no_mes = false;
    bool npos_reached = false;
    bool mes_reached = false;

    int rn_pos    = -1; // "\r\n"
    int space_pos = -1; // " "
    int col_pos   = -1; // ":"
    int excl_pos  = -1; // "!"
    int at_pos    = -1; // "@"


    sf::Socket::Status status;
    sf::TcpSocket      socket;


    std::string connection_password;
    std::string nickname;
    std::string username;
    std::string realname;
    std::string mode;


    std::string formated_text;
    std::string target_channel;
    std::string server;
    int port;

    //Common Commands used in IRC
    static const std::string PONG;
    static const std::string PING;
    static const std::string PASS;
    static const std::string PRIVMSG;
    static const std::string USER;
    static const std::string NICK;
    static const std::string JOIN;
    static const std::string QUIT;

    //Common characters and \r\n
    static const std::string unused;
    static const std::string s; //space
    static const std::string c; //colon
    static const std::string rn;

    //Dyrand
    static const std::string Dyrand;
};



#endif // BOT_CLASS_HPP

