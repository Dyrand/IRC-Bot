#ifndef BOT_CLASS_HPP
#define BOT_CLASS_HPP

#include <SFML/Network.hpp>

#include "serverMessageStruct.hpp"
#include "ircMimic.hpp"


// TODO (dyrand#4#12/15/2014): fun command, detects if there is fun

class Bot
{
    public:

        Bot();

        //Establishing Connections
        int connectServer(std::string server_t, int port_t);
        int connectServer();
        void registerConnection(std::string conn_pwd_t, std::string nick_t, std::string username_t );
        void registerConnection();

        //Outputs text depending on what status is
        int updateStatus(sf::Socket::Status status_t);
        int outputStatus();

        //Send and Receive
        void send(std::string formated_text);
        void receive();

        //Basic IRC commands
        void privmsg(std::string channel_t, std::string send_msg_t);
        void privmsg(std::string msg_t);
        void join(std::string channel_t);
        void join();
        void part(std::string channel_t);
        void quit(std::string msg_t);
        void quit();

        void parseServerMsg();
        void parseMsg();
        void postHandler();
        void checkCommands();
        void serverResponse();
        void resetVars();
        void loop();

        /*Simple Commands*/ //most likely can remain in ircBotClass
        void discon();
        void rawInput();
        void join_c();
        void part_c();

        /*Objects for commands*/
        ircMimic mimic_o;



        /*Get functions*/
        std::string getNick();
        void getMsgStructs(serverMsgStruct& s_msg_struct_t, msgStruct& msg_struct_t);

    private:

        /*Message Structure objects*/
        serverMsgStruct s_msg_struct; //server_message_struct
        msgStruct         msg_struct; //message_struct

        bool stay_connected;

        long unsigned int bytes_rec;
        std::array<char,512> rec_text;
        std::vector<std::string> parsable_strings;
        std::string rec_string;

        std::string part_string;
        std::string temp_string;
        unsigned int string_length;

        bool partial_flag = false;

        size_t rn_pos    = std::string::npos; // "\r\n"
        size_t space_pos = std::string::npos; // " "
        size_t excl_pos  = std::string::npos; // "!"
        size_t at_pos    = std::string::npos; // "@"

        sf::Socket::Status status;
        sf::TcpSocket      socket;
        
        std::string connection_password;
        std::string conn_pwd;
        std::string nick;
        std::string user;
        std::string real;
        std::string mode;

        std::string send_msg;
        std::string target_channel;
        std::string channel;
        std::string target_server;
        std::string server;
        int target_port;

        //Dyrand
        const std::string Dyrand = "Dyrand";
};



#endif // BOT_CLASS_HPP

