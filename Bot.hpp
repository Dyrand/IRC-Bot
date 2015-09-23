#ifndef BOT_CLASS_HPP
#define BOT_CLASS_HPP

#include <SFML/Network.hpp>

#include "TcpSocket.hpp"
#include "serverMessageStruct.hpp"
#include "ircMimic.hpp"


// TODO (dyrand#4#12/15/2014): fun command, detects if there is fun

class Bot
{
    public:

        Bot();

        //Connection establishers
        sf::Socket::Status connectToServer(std::string serverName_t, short int port_t);
        sf::Socket::Status connectToServer();
        void registerConnection(std::string connection_password_t, std::string nickname_t, std::string username_t, std::string realname_t, std::string mode);
        void registerConnection();

        //Send and Receive
        sf::Socket::Status send(std::string formated_text);
        void receive();

        //Basic IRC commands
        void privmsg(std::string channel_t, std::string sendMessage_t);
        void privmsg(std::string sendMessage_t);
        void join(std::string channel_t);
        void join();
        void part(std::string channel_t, std::string sendMessage_t);
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
        std::string getNickname();
        void getMsgStructs(serverMsgStruct& s_msg_struct_t, msgStruct& msg_struct_t);

    private:

        /*Message Structure objects*/
        serverMsgStruct s_msg_struct; //server_message_struct
        msgStruct         msg_struct; //message_struct

        bool stay_connected;
        sf::Time maxTimeToWait;

        std::size_t bytes_rec;
        std::array<char,512> text_received;
        std::vector<std::string> parsable_strings;
        std::string string_received;

        std::string partial_string;
        std::string temp_string;
        
        unsigned int string_length;

        bool isStringPartial = false;
        bool isStatusOutputOpen = false;
        
        size_t rn_pos    = std::string::npos; // "\r\n"
        size_t space_pos = std::string::npos; // " "
        size_t excl_pos  = std::string::npos; // "!"
        size_t at_pos    = std::string::npos; // "@"

        TcpSocket socket;
        
        std::string connectionPassword;
        std::string nickname;
        std::string user;
        std::string realname;
        std::string mode;

        std::string quitMessage;
        std::string sendMessage;
        std::string targetChannel;
        std::string channel;
        std::string targetServer;
        std::string server;
        unsigned short targetPort;

        //Dyrand
        const std::string Dyrand = "Dyrand";
};



#endif // BOT_CLASS_HPP
