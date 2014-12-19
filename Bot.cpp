#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdlib>

#include <SFML/Network.hpp>

#include "Bot.hpp"
#include "serverMessageStruct.hpp"


Bot::Bot():
    mimic_o(this),
    stay_conned(true),
    conn_pwd("password"),
    nick("Dyramica"),
    user("dyramic"),
    real("dyramic"),
    mode("0"),
    channel("#botdever"),
    server("irc.freenode.com"),
    port(8001)
{}

int Bot::connectToServer(std::string server_t, int port_t)
{
    server = server_t;
    port   = port_t;
    return connectToServer();
}

int Bot::connectToServer()
{return statusSwitch(status = socket.connect(server,port));}

void Bot::connectionRegistration(std::string conn_pwd_t, std::string nick_t, std::string user_t)
{
    conn_pwd = conn_pwd_t;
    nick = nick_t;
    user = user_t;
    return connectionRegistration();
}

void Bot::connectionRegistration()
{
    send("PASS "+conn_pwd);
    send("NICK "+nick);
    send("USER "+user+" "+mode+" * :"+real);
}

int Bot::statusSwitch(sf::Socket::Status status_t)
{
    status = status_t;
    return statusSwitch();
}

int Bot::statusSwitch()
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


void Bot::send(std::string send_mes)
{
    send_mes.append("\r\n");
    std::cout << send_mes;
    status = socket.send(send_mes.c_str(),send_mes.size());
}

//Using A TCPsocket, divide a message based on "\r\n" position, messages with out a "\r\n" at the end is regarded as
//a partial string and saved for combining
void Bot::receive()
{
    status = socket.receive(rec_text.data(),rec_text.size(),bytes_rec);
    rec_string.assign(rec_text.begin(),rec_text.begin()+bytes_rec);

    //Appends two partial strings together if necessary
    rn_pos = rec_string.find("\r\n");
    if(part_flag && (rn_pos != rec_string.npos))
    {
        parsable_strings.emplace_back(partial_string+(rec_string.substr(0,rn_pos+2)));
        rec_string.erase(0,rn_pos+2);
        partial_string.clear();
        part_flag = false;
    }

    //Split strings on "\r\n" positions and removes characters until there is no "\r\n" left
    while((rn_pos = rec_string.find("\r\n")) != rec_string.npos)
    {
        parsable_strings.emplace_back(rec_string.substr(0,rn_pos+2));
        rec_string.erase(0,rn_pos+2);
    }

    //If there is no "\r\n" left, but there are charcters still left in the string, it is a partial string
    //that must be combined
    if(rec_string.size() != 0)
    {
        partial_string.assign(rec_string);
        rec_string.clear();
        part_flag = true;
    }

    rec_text.fill('\0');
}

void Bot::privmsg(std::string channel, std::string message)
{send("PRIVMSG "+channel+" :"+message);}

void Bot::join(std::string channel_t)
{
    channel = channel_t;
    join();
}

void Bot::join()
{send("JOIN "+channel);}

void Bot::quit(std::string mes)
{send("QUIT "+mes);}



void Bot::parseServerMes()
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



    std::cout << rec_string;

    if((rn_pos = rec_string.find("\r\n")) == std::string::npos)
    {std::cout << "Receive string does not contain \"\\r\\n\".";}

    //Gets the prefix if there is one
    if(rec_string.front() == ':')
    {
        if((s_mes_struct.prefix_end_pos = rec_string.find(' ')) != std::string::npos)
        {s_mes_struct.prefix = rec_string.substr(1,s_mes_struct.prefix_end_pos);}
        else{std::cout << "Receive string does not contain a space.\n";}
    }else{std::cout << "Receive string does not start with a ':'.\n";}


    //Gets the message if there is one
    if((s_mes_struct.mes_start_pos = rec_string.find(" :")) != std::string::npos)
    {s_mes_struct.message = rec_string.substr(s_mes_struct.mes_start_pos+2,(rn_pos-s_mes_struct.mes_start_pos)-2);}
    else
    {
        std::cout << "Receive string does not contain a message.\n";
        //padding for correctly getting the last block of text
        rec_string.insert(rn_pos," ");
        s_mes_struct.mes_start_pos = rn_pos;
    }


    //Find all the spaces between prefix_end_pos and mes_start_pos
    space_pos = s_mes_struct.prefix_end_pos;
    while((space_pos = rec_string.find(' ',++space_pos)) < s_mes_struct.mes_start_pos)
    {s_mes_struct.space_pos.emplace_back(space_pos);}

    s_mes_struct.space_pos.emplace_back(s_mes_struct.mes_start_pos);

    //Gets the command in the message, at least there should be one
    s_mes_struct.command = rec_string.substr(s_mes_struct.prefix_end_pos+1,(s_mes_struct.space_pos.at(0)-s_mes_struct.prefix_end_pos)-1);


    //Implies the 2nd thing after the message is a channel or nickname, both go to nickname var though
    if(s_mes_struct.space_pos.size() >= 2)
    {s_mes_struct.channel = rec_string.substr(s_mes_struct.space_pos.at(0)+1,s_mes_struct.space_pos.at(1)-s_mes_struct.space_pos.at(0));}

    //Parse the prefix into nickname and username if possible
    if((excl_pos = s_mes_struct.prefix.find('!')) != std::string::npos)
    {s_mes_struct.nick = s_mes_struct.prefix.substr(0,excl_pos);}
    if((at_pos = s_mes_struct.prefix.find('@')) != std::string::npos)
    {
        if(s_mes_struct.prefix.at(excl_pos+1) == '~')
        {s_mes_struct.user = s_mes_struct.prefix.substr(excl_pos+2,(at_pos-excl_pos)-2);}
        else
        {s_mes_struct.user = s_mes_struct.prefix.substr(excl_pos+1,(at_pos-excl_pos)-1);}
    }

    //The rest of the stuff left between spaces is put into a string vector
    for(int i(1); i < (s_mes_struct.space_pos.size()-1); i++)
    {s_mes_struct.args.emplace_back(rec_string.substr(s_mes_struct.space_pos.at(i)+1,s_mes_struct.space_pos.at(i+1)-s_mes_struct.space_pos.at(i)));}

    //std::cout << "prefix  :" << s_mes_struct.prefix   << rn;
    //std::cout << "command :" << s_mes_struct.command  << rn;
    //std::cout << "channel :" << s_mes_struct.channel  << rn;
    //std::cout << "message :" << s_mes_struct.message  << rn;
    //std::cout << "nick:" << s_mes_struct.nick << rn;
    //std::cout << "user:" << s_mes_struct.user << rn;

    //for(int i(0); i < s_mes_struct.args.size(); i++)
    //{std::cout << "Argument"<< i << ":" << s_mes_struct.args.at(i) << rn;}
}


void Bot::postHandler()
{
    if(mimic_o.mimicing())
    {mimic_o.mimic_handler();}
}


void Bot::parseMes()
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
    {discon();}
    else if(mes_struct.command == ">")
    {rawInput();}
    //else
    //{privmsg(s_mes_struct.channel,"That is not a command I know.");}
}

void Bot::serverResponse()
{

    if(s_mes_struct.command=="PING")
    {send("PONG "+s_mes_struct.message+"\r\n");}
}



void Bot::resetVars()
{
    s_mes_struct.reset();
    mes_struct.reset();
}


void Bot::loop()
{

    do
    {
        receive();
        for(int i(0); i < parsable_strings.size(); i++)
        {
            rec_string = parsable_strings.at(i);
            if(!rec_string.empty())
            {
                parseServerMes();
                parseMes();

                if(!mes_struct.command.empty())
                {checkCommands();}

                serverResponse();
                postHandler();
                resetVars();
            }
        }parsable_strings.clear();
    }while(stay_conned==true);
    quit("Ever want to look more like?");
    socket.disconnect();
}

void Bot::discon()
{
    if(s_mes_struct.nick == Dyrand)
    {stay_conned = false;}
}

void Bot::rawInput()
{

    if((s_mes_struct.nick == Dyrand) && (mes_struct.message.size() > 2))
    {
    mes_struct.postfix = mes_struct.message.substr(3,mes_struct.last_char_pos-1);
    send(mes_struct.postfix+"\r\n");
    }
    else
    {privmsg(s_mes_struct.channel,"\1ACTION tells "+s_mes_struct.nick+" that the \">>\" command is only for: "+Dyrand+".\1");}
}

std::string Bot::getnick()
{return nick;}


