#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdlib>

#include <SFML/Network.hpp>

#include "Bot.hpp"
#include "serverMessageStruct.hpp"


Bot::Bot():
    mimic_o(this),
    stay_connected(true),
    connection_password("password"),
    nickname("Dyramic"),
    username("dyramic"),
    realname("dyramic"),
    mode("0"),
    target_channel("#botdever"),
    server("irc.freenode.com"),
    port(8001)
{}

int Bot::connectToServer(std::string server, int port)
{return statusSwitch(status = socket.connect(server,port));}

int Bot::connectToServer()
{return statusSwitch(status = socket.connect(server,port));}


int Bot::connectionRegistration()
{
    using namespace irc;
    send(PASS+s+connection_password);
    send(NICK+s+nickname);
    send(USER+s+username+s+mode+s+unused+s+c+realname);
    return status;
}

int Bot::statusSwitch(int status)
{
    switch(status)
    {
    case sf::Socket::Done:
        std::cout << "Connection to " << server << " done.\n";
        return sf::Socket::Done;
        break;
    case sf::Socket::NotReady:
        std::cout << "Connection to " << server << "not ready.\n";
        return sf::Socket::NotReady;
        break;
    case sf::Socket::Disconnected:
        std::cout << "Disconnected from " << server << ".\n";
        return sf::Socket::Disconnected;
        break;
    case sf::Socket::Error:
    default:
        std::cout << "Error connecting to " << server << ".\n";
        return sf::Socket::Error;
        break;
    }
}


void Bot::send(std::string formated_text)
{
    using namespace irc;
    formated_text.append(rn);
    std::cout << formated_text;
    status = socket.send(formated_text.c_str(),formated_text.size());
}

//Using A TCPsocket, divide a message based on "\r\n" position, messages with out a "\r\n" at the end is regarded as
//a partial string and saved for combining
void Bot::receive()
{
    using namespace irc;

    status = socket.receive(receive_text.data(),receive_text.size(),bytes_received);
    receive_string.assign(receive_text.begin(),receive_text.begin()+bytes_received);

    //Appends two partial strings together if necessary
    rn_pos = receive_string.find(rn);
    if(partial_flag && (rn_pos != receive_string.npos))
    {
        parsable_strings.emplace_back(partial_string+(receive_string.substr(0,rn_pos+2)));
        receive_string.erase(0,rn_pos+2);
        partial_string.clear();
        partial_flag = false;
    }

    //Split strings on "\r\n" positions and removes characters until there is no "\r\n" left
    while((rn_pos = receive_string.find(rn)) != receive_string.npos)
    {
        parsable_strings.emplace_back(receive_string.substr(0,rn_pos+2));
        receive_string.erase(0,rn_pos+2);
    }

    //If there is no "\r\n" left, but there are charcters still left in the string, it is a partial string
    //that must be combined
    if(receive_string.size() != 0)
    {
        partial_string.assign(receive_string);
        receive_string.clear();
        partial_flag = true;
    }

    receive_text.fill('\0');
}


void Bot::join(std::string target_channel)
{
    using namespace irc;
    send(JOIN+" "+target_channel);
}

void Bot::join()
{
    using namespace irc;
    send(JOIN+" "+target_channel);
}

void Bot::privmsg(std::string channel, std::string message)
{
    using namespace irc;
    send(PRIVMSG+" "+channel+" "+" :"+message);
}



void Bot::parserOfServerMessages()
{
//<-----------prefix-----------> <command> <channel> :<message>
//<nick|servername>!<username>@ <command> <channel> :<message>
//:Dyrand!~Dyrand@unaffiliated/dyrand PRIVMSG #botdever :9

//<nick|servername>!<username>@ <command> :<message>
//:Dyrand!~Dyrand@unaffiliated/dyrand QUIT :Client Closed
//:QKiller!~usandbots@unaffiliated/usandfriends JOIN #botdever

//:sendak.freenode.net 004 Dyramic sendak.freenode.net ircd-seven-1.1.3 DOQRSZaghilopswz CFILMPQSbcefgijklmnopqrstvz bkloveqjfI

//PING :<pong>

//ERROR :Closing Link: 127.0.0.1 (Connection timed out)
//HYPOTHETICALMESSAGE  /*no space and no : or !

    using namespace irc;

    std::cout << receive_string;

    if((rn_pos = receive_string.find("\r\n")) == std::string::npos)
    {std::cout << "Receive string does not contain \"\\r\\n\".";}

    //Gets the prefix if there is one
    if(receive_string.front() == ':')
    {
        if((s_mes_struct.prefix_end_pos = receive_string.find(' ')) != std::string::npos)
        {s_mes_struct.prefix = receive_string.substr(1,s_mes_struct.prefix_end_pos);}
        else{std::cout << "Receive string does not contain a space.\n";}
    }else{std::cout << "Receive string does not start with a ':'.\n";}


    //Gets the message if there is one
    if((s_mes_struct.mes_start_pos = receive_string.find(" :")) != std::string::npos)
    {s_mes_struct.message = receive_string.substr(s_mes_struct.mes_start_pos+2,(rn_pos-s_mes_struct.mes_start_pos)-2);}
    else
    {
        std::cout << "Receive string does not contain a message.\n";
        //padding for correctly getting the last block of text
        receive_string.insert(rn_pos,s);
        s_mes_struct.mes_start_pos = rn_pos;
    }


    //Find all the spaces between prefix_end_pos and mes_start_pos
    space_pos = s_mes_struct.prefix_end_pos;
    while((space_pos = receive_string.find(' ',++space_pos)) < s_mes_struct.mes_start_pos)
    {s_mes_struct.space_pos.emplace_back(space_pos);}

    s_mes_struct.space_pos.emplace_back(s_mes_struct.mes_start_pos);

    //Gets the command in the message, at least there should be one
    s_mes_struct.command = receive_string.substr(s_mes_struct.prefix_end_pos+1,(s_mes_struct.space_pos.at(0)-s_mes_struct.prefix_end_pos)-1);


    //Implies the 2nd thing after the message is a channel or nickname, both go to nickname var though
    if(s_mes_struct.space_pos.size() >= 2)
    {s_mes_struct.channel = receive_string.substr(s_mes_struct.space_pos.at(0)+1,s_mes_struct.space_pos.at(1)-s_mes_struct.space_pos.at(0));}

    //Parse the prefix into nickname and username if possible
    if((excl_pos = s_mes_struct.prefix.find('!')) != std::string::npos)
    {s_mes_struct.nickname = s_mes_struct.prefix.substr(0,excl_pos);}
    if((at_pos = s_mes_struct.prefix.find('@')) != std::string::npos)
    {
        if(s_mes_struct.prefix.at(excl_pos+1) == '~')
        {s_mes_struct.username = s_mes_struct.prefix.substr(excl_pos+2,(at_pos-excl_pos)-2);}
        else
        {s_mes_struct.username = s_mes_struct.prefix.substr(excl_pos+1,(at_pos-excl_pos)-1);}
    }

    //The rest of the stuff left between spaces is put into a string vector
    for(int i(1); i < (s_mes_struct.space_pos.size()-1); i++)
    {s_mes_struct.args.emplace_back(receive_string.substr(s_mes_struct.space_pos.at(i)+1,s_mes_struct.space_pos.at(i+1)-s_mes_struct.space_pos.at(i)));}

    //std::cout << "prefix  :" << s_mes_struct.prefix   << rn;
    //std::cout << "command :" << s_mes_struct.command  << rn;
    //std::cout << "channel :" << s_mes_struct.channel  << rn;
    //std::cout << "message :" << s_mes_struct.message  << rn;
    //std::cout << "nickname:" << s_mes_struct.nickname << rn;
    //std::cout << "username:" << s_mes_struct.username << rn;

    //for(int i(0); i < s_mes_struct.args.size(); i++)
    //{std::cout << "Argument"<< i << ":" << s_mes_struct.args.at(i) << rn;}
}


void Bot::postHandler()
{
    if(mimic_o.mimicing())
    {mimic_o.mimic_handler();}
}


void Bot::parserOfMessages()
{
    //Check if message is empty
    if(s_mes_struct.message.empty())
    {return;}

    mes_struct.message = s_mes_struct.message;
    mes_struct.last_char_pos = mes_struct.message.length()-1;

    //Check if the message has the identifier for a command, stored in mes_struct.ident
    if(mes_struct.message.at(0) != mes_struct.ident)
    {return;}

    //Find the spaces within the message
    space_pos = 0;
    while((space_pos = mes_struct.message.find(' ',++space_pos)) != std::string::npos)
    {mes_struct.space_pos.emplace_back(space_pos);}

    //Adds a padding space for the end of the message for substr
    mes_struct.space_pos.emplace_back(mes_struct.space_pos.size());

    //Simple substr to get command
    mes_struct.command = mes_struct.message.substr(1,(mes_struct.space_pos.at(0)-1));

    bool modifier_present = true;
    int i(0);
    //Parsing modifiers
    for(; (i < (mes_struct.space_pos.size()-1)) & modifier_present; i++)
    {
        temp_string = mes_struct.message.substr(mes_struct.space_pos.at(i)+1,((mes_struct.space_pos.at(i+1)-mes_struct.space_pos.at(i)))-1);

        if(!temp_string.empty())
        {
            if(temp_string.at(0)==mes_struct.minu || temp_string.at(0)==mes_struct.plu)
            {mes_struct.mods.push_back(temp_string);}
            else
            {
                modifier_present = false;
                --i; //So argument parser can get the string instead
            }
        }
    }
    //Parsing arguments
    for(; i < (mes_struct.space_pos.size()-1); i++)
    {
        temp_string = mes_struct.message.substr(mes_struct.space_pos.at(i)+1,((mes_struct.space_pos.at(i+1)-mes_struct.space_pos.at(i))-1));

        if(!temp_string.empty())
        {mes_struct.args.push_back(temp_string);}
    }
    mes_struct.space_pos.pop_back(); //Remove the space padding

    //std::cout << "command:" << mes_struct.command << rn;
    //for(int i(0); i < mes_struct.args.size(); i++)
    //{std::cout << "Argument"<< i << ":" << mes_struct.args.at(i) << rn;}
    //for(int i(0); i < mes_struct.mods.size(); i++)
    //{std::cout << "Modifier"<< i << ":" << mes_struct.mods.at(i) << rn;}
}


void Bot::checkCommands()
{
    if(mes_struct.command == "mimic")
    {mimic_o.mimic();}
    else if(mes_struct.command == "demimic")
    {mimic_o.demimic();}
    else if(mes_struct.command == "discon")
    {disconnect();}
    else if(mes_struct.command == ">")
    {rawInput();}
    //else
    //{privmsg(s_mes_struct.channel,"That is not a command I know.");}
}

void Bot::serverResponse()
{
    using namespace irc;
    if(s_mes_struct.command==PING)
    {send(PONG+s+s_mes_struct.message+rn);}
}



void Bot::resetVars()
{
    s_mes_struct.reset();
    mes_struct.reset();
}


void Bot::loop()
{
    using namespace irc;
    do
    {
        receive();
        for(int i(0); i < parsable_strings.size(); i++)
        {
            receive_string = parsable_strings.at(i);
            if(!receive_string.empty())
            {
                parserOfServerMessages();
                parserOfMessages();

                if(!mes_struct.command.empty())
                {checkCommands();}

                serverResponse();
                postHandler();
                resetVars();
            }
        }parsable_strings.clear();
    }while(stay_connected==true);
    send(QUIT+" :Ever want to look more like?"+rn);
    socket.disconnect();
}

void Bot::disconnect()
{
    if(s_mes_struct.nickname == Dyrand)
    {stay_connected = false;}
}

void Bot::rawInput()
{
    using namespace irc;
    if((s_mes_struct.nickname == Dyrand) && (mes_struct.message.size() > 2))
    {
    mes_struct.postfix = mes_struct.message.substr(3,mes_struct.last_char_pos-1);
    send(mes_struct.postfix+rn);
    }
    else
    {privmsg(s_mes_struct.channel,"\1ACTION tells "+s_mes_struct.nickname+" that the \">>\" command is only for: "+Dyrand+".\1");}
}

std::string Bot::getNickname()
{return nickname;}


