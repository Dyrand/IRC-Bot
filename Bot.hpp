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
        int connectToServer(std::string server_t, int port_t);
        int connectToServer();
        void connectionRegistration(std::string conn_pwd_t, std::string nick_t, std::string username_t );
        void connectionRegistration();

        //Outputs text depending on what status is
        int statusSwitch(sf::Socket::Status status_t);
        int statusSwitch();

        //Send and Receive
        void send(std::string formated_text);
        void receive();

        //Basic IRC commands
        void privmsg(std::string channel, std::string mes);
        void join(std::string target_channel);
        void join();
        void quit(std::string mes);

        void parseServerMes();
        void postHandler();
        void parseMes();
        void checkCommands();
        void serverResponse();
        void resetVars();
        void loop();

        /*Simple Commands*/ //most likely can remain in ircBotClass
        void discon();
        void rawInput();

        /*Objects for commands*/
        ircMimic mimic_o;


        serverMessageStruct s_mes_struct; //server_message_struct
        messageStruct         mes_struct; //message_struct

        /*Get functions*/
        std::string getnick();

    private:

        bool stay_conned;


        long unsigned int bytes_rec;
        std::array<char,512> rec_text;
        std::vector<std::string> parsable_strings;
        std::string rec_string;

        std::string partial_string;
        std::string temp_string;

        bool part_flag = false;

        int rn_pos    = std::string::npos; // "\r\n"
        int space_pos = std::string::npos; // " "
        int excl_pos  = std::string::npos; // "!"
        int at_pos    = std::string::npos; // "@"


        sf::Socket::Status status;
        sf::TcpSocket      socket;


        std::string conn_pwd;
        std::string nick;
        std::string user;
        std::string real;
        std::string mode;


        std::string send_mes;
        std::string channel;
        std::string server;
        int port;

        //Dyrand
        const std::string Dyrand = "Dyrand";
};



#endif // BOT_CLASS_HPP

