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
    nickname("Dyramica"),
    user("dyramic"),
    real("dyramic"),
    mode("0"),
    target_channel("botdever"),
    target_server("irc.freenode.com"),
    target_port(8001)
{}

int Bot::connectServer(std::string server_t, int port_t)
{
    target_server = server_t;
    target_port   = port_t;
    return connectServer();
}

int Bot::connectServer()
{
    updateStatus(socket.connect(target_server,target_port));
    return outputStatus();
}

void Bot::registerConnection(std::string connection_password_t, std::string nickname_t, std::string user_t, std::string realname_t)
{
    connection_password = connection_password_t;
    nickname = nickname_t;
    user = user_t;
    realname = realname_t;
    registerConnection();
}

void Bot::registerConnection()
{
    send("PASS "+connection_password);
    send("NICK "+nickname);
    send("USER "+user+" "+mode+" * :"+realname);
}

int Bot::updateStatus(sf::Socket::Status status_t)
{
    status = status_t;
    return outputStatus();
}

int Bot::outputStatus()
{
    if(isStatusOutputOpen)
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
}

int Bot::send(std::string send_message_t)
{
    send_message = send_message_t;
    send_message.append("\r\n");
    std::cout << send_message;
    updateStatus(socket.send(send_message.c_str(),send_message.size());
    return status;
}

//Using A TCPsocket, divide a message based on "\r\n" position, messages without a "\r\n" at the end is regarded as
//a partial string and saved for combining
void Bot::receive()
{
    updateStatus(socket.receive(text_received.data(),text_received.size(),bytes_rec));
    recieve_string.assign(text_received.begin(),text_received.begin()+bytes_rec);

    //Appends one partial strings to another
    rn_pos = string_received.find("\r\n");
    if(isStringPartial && (rn_pos != string_received.npos))
    {
        temp_string = string_received.substr(0,rn_pos+2);
        parsable_strings.emplace_back(partial_string+temp_string);
        string_received.erase(0,rn_pos+2);
        partial_string.clear();
        isStringPartial = false;
    }

    //Split strings on "\r\n" positions and removes characters until there is no "\r\n" left
    while((rn_pos = string_received.find("\r\n")) != string_received.npos)
    {
        temp_string = string_received.substr(0,rn_pos+2)
        parsable_strings.emplace_back(temp_string);
        string_received.erase(0,rn_pos+2);
    }

    //If there is no "\r\n" left, but there are charcters still left in the string, it is a partial string
    if(string_received.empty() == false)
    {
        partial_string.assign(string_received);
        string_received.clear();
        isStringPartial = true;
    }

    text_received.fill('\0');
}

void Bot::privmsg(std::string channel_t, std::string send_message_t)
{
    target_channel = channel_t;
    privmsg(send_message_t);
}

void Bot::privmsg(std::string send_message_t)
{
    send_message = send_message_t;
    send("PRIVMSG "+target_channel+" :"+send_message);
}

void Bot::join(std::string channel_t)
{
    target_channel = channel_t;
    join();
}

void Bot::join()
{send("JOIN "+target_channel);}

void Bot::part(std::string channel_t)
{
    channel = channel_t;
    send("PART "+channel);
}

void Bot::quit(std::string send_message_t)
{
    send_message = send_message_t;
    send("QUIT "+send_message);
}

void Bot::quit()
{
    send("QUIT gotta go fast!");
}


void Bot::parseServerMsg()
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

    std::cout << string_received;

    if((rn_pos = string_received.find("\r\n")) == std::string::npos)
    {std::cout << "Receive string does not contain \"\\r\\n\".";}

    //Gets the prefix if there is one
    if(string_received.front() == ':')
    {
        if((s_msg_struct.prefix_end_pos = string_received.find(' ')) != std::string::npos)
        {s_msg_struct.prefix = string_received.substr(1,s_msg_struct.prefix_end_pos);}
        else{std::cout << "Receive string does not contain a space.\n";}
    }else{std::cout << "Receive string does not start with a ':'.\n";}

    //Gets the message if there is one
    if((s_msg_struct.msg_start_pos = string_received.find(" :")) != std::string::npos)
    {s_msg_struct.msg = string_received.substr(s_msg_struct.msg_start_pos+2,(rn_pos-s_msg_struct.msg_start_pos)-2);}
    else
    {
        std::cout << "Receive string does not contain a message.\n";
        //padding for correctly getting the last block of text
        string_received.insert(rn_pos," ");
        s_msg_struct.msg_start_pos = rn_pos;
    }

    //Find all the spaces between prefix_end_pos and mes_start_pos
    space_pos = s_msg_struct.prefix_end_pos;
    while((space_pos = string_received.find(' ',++space_pos)) < s_msg_struct.msg_start_pos)
    {s_msg_struct.space_pos.emplace_back(space_pos);}

    //Padding for collecting last string in string_received apart from message
    s_msg_struct.space_pos.emplace_back(s_msg_struct.msg_start_pos);

    //Gets the command in the message
    s_msg_struct.command = string_received.substr(s_msg_struct.prefix_end_pos+1,(s_msg_struct.space_pos.at(0)-s_msg_struct.prefix_end_pos)-1);

    //Implies the 2nd piece after the message is a channel or nickname, both go to channel var though
    if(s_msg_struct.space_pos.size() >= 2)
    {s_msg_struct.channel = string_received.substr(s_msg_struct.space_pos.at(0)+1,(s_msg_struct.space_pos.at(1)-s_msg_struct.space_pos.at(0))-1);}

    //Parse the prefix into nickname and username if possible
    if((excl_pos = s_msg_struct.prefix.find('!')) != std::string::npos)
    {s_msg_struct.nickname = s_msg_struct.prefix.substr(0,excl_pos);}
    if((at_pos = s_msg_struct.prefix.find('@')) != std::string::npos)
    {
        if(s_msg_struct.prefix.at(excl_pos+1) == '~')
        {s_msg_struct.user = s_msg_struct.prefix.substr(excl_pos+2,(at_pos-excl_pos)-2);}
        else
        {s_msg_struct.user = s_msg_struct.prefix.substr(excl_pos+1,(at_pos-excl_pos)-1);}
    }

    //The rest of the stuff left between spaces is put into a string vector
    for(unsigned int i(1); i < (s_msg_struct.space_pos.size()-1); i++)
    {s_msg_struct.args.emplace_back(string_received.substr(s_msg_struct.space_pos.at(i)+1,s_msg_struct.space_pos.at(i+1)-s_msg_struct.space_pos.at(i)));}



    //Check if nick_flag should be set and what the msg_target should be
    if(s_msg_struct.channel.find('#') == std::string::npos)
    {
        s_msg_struct.nickname_flag = true;
        s_msg_struct.adjust_channel();
    }
    else
    {s_msg_struct.msg_target = s_msg_struct.channel;}

    //std::cout << "prefix  :" << s_msg_struct.prefix   << "\n";
    //std::cout << "command :" << s_msg_struct.command  << "\n";
    //std::cout << "channel :" << s_msg_struct.channel  << "\n";
    //std::cout << "message :" << s_msg_struct.msg  << "\n";
    //std::cout << "nick:" << s_msg_struct.nick << "\n";
    //std::cout << "user:" << s_msg_struct.user << "\n";

    //for(unsigned int i(0); i < s_msg_struct.args.size(); i++)
    //{std::cout << "Argument"<< i << ":" << s_msg_struct.args.at(i) << "\n";}
}


void Bot::postHandler()
{
    if(mimic_o.mimicing())
    {mimic_o.mimic_handler();}
}


void Bot::parseMsg()
{
    //Check if message is empty
    if(s_msg_struct.msg.empty())
    {return;}

    msg_struct.msg = s_msg_struct.msg;
    msg_struct.last_char_pos = msg_struct.msg.length()-1;

    //Check if the message has the identifier for a command, stored in msg_struct.ident
    if(msg_struct.msg.at(0) != msg_struct.ident)
    {return;}

    //Find the spaces within the message
    space_pos = 0;
    while((space_pos = msg_struct.msg.find(' ',++space_pos)) != std::string::npos)
    {msg_struct.space_pos.emplace_back(space_pos);}

    //Adds a padding space for the end of the message for substr
    msg_struct.space_pos.emplace_back(msg_struct.space_pos.size());

    //Simple substr to get command
    msg_struct.command = msg_struct.msg.substr(1,(msg_struct.space_pos.at(0)-1));

    bool modifier_present = true;
    unsigned int i(0);
    //Parsing modifiers
    for(; (i < (msg_struct.space_pos.size()-1)) && modifier_present; i++)
    {
        temp_string = msg_struct.msg.substr(msg_struct.space_pos.at(i)+1,((msg_struct.space_pos.at(i+1)-msg_struct.space_pos.at(i)))-1);

        if(!temp_string.empty())
        {
            if(temp_string.at(0)==msg_struct.minu || temp_string.at(0)==msg_struct.plu)
            {msg_struct.mods.emplace_back(temp_string);}
            else
            {
                modifier_present = false;
                --i; //So argument parser can get the string instead
            }
        }
    }
    //Parsing arguments
    for(; i < (msg_struct.space_pos.size()-1); i++)
    {
        temp_string = msg_struct.msg.substr(msg_struct.space_pos.at(i)+1,(msg_struct.space_pos.at(i+1)-msg_struct.space_pos.at(i))-1);

        if(!temp_string.empty())
        {msg_struct.args.push_back(temp_string);}
    }
    msg_struct.space_pos.pop_back(); //Remove the space padding

    //Gets the postfix if there is one
    if((string_length = msg_struct.command.size()) != msg_struct.last_char_pos)
    {msg_struct.postfix = msg_struct.msg.substr(string_length+2,(msg_struct.last_char_pos-string_length)-1);}

    //std::cout << "command:" << msg_struct.command << "\n";
    //std::cout << "postfix:" << msg_struct.postfix << "\n";
    //for(int i(0); i < msg_struct.args.size(); i++)
    //{std::cout << "Argument"<< i << ":" << msg_struct.args.at(i) << "\n";}
    //for(int i(0); i < msg_struct.mods.size(); i++)
    //{std::cout << "Modifier"<< i << ":" << msg_struct.mods.at(i) << "\n";}
}

void Bot::checkCommands()
{
    if(msg_struct.command == "mimic")
    {mimic_o.mimic();}
    else if(msg_struct.command == "demimic")
    {mimic_o.demimic();}
    else if(msg_struct.command == "discon")
    {discon();}
    else if(msg_struct.command == "join")
    {join_c();}
    else if(msg_struct.command == "part")
    {part_c();}
    else if(msg_struct.command == ">")
    {rawInput();}
}

void Bot::serverResponse()
{
    if(s_msg_struct.command=="PING")
    {send("PONG "+s_msg_struct.msg+"\r\n");}
}



void Bot::resetVars()
{
    s_msg_struct.reset();
    msg_struct.reset();
}


void Bot::loop()
{
    do
    {
        receive();
        for(unsigned int i(0); i < parsable_strings.size(); i++)
        {
            string_received = parsable_strings.at(i);
            if(!string_received.empty())
            {
                parseServerMsg();
                parseMsg();

                if(!msg_struct.command.empty())
                {checkCommands();}

                serverResponse();
                postHandler();
                resetVars();
            }
        }parsable_strings.clear();
    }while(stay_connected==true);
    quit("Ever want to look more like?");
    socket.disconnect();
}

void Bot::discon()
{
    if(s_msg_struct.nick == Dyrand)
    {stay_connected = false;}
}

void Bot::join_c()
{
    if(s_msg_struct.nickname == Dyrand)
    {
        for(unsigned int i(0);  i < msg_struct.args.size(); i++)
        {join(msg_struct.args.at(i));}
    }
}

void Bot::part_c()
{
    if(s_msg_struct.nickname == Dyrand)
    {
        for(unsigned int i(0);  i < msg_struct.args.size(); i++)
        {part(msg_struct.args.at(i));}
    }
}

void Bot::rawInput()
{
    if(s_msg_struct.nickname == Dyrand)
    {send(msg_struct.postfix);}
    else
    {privmsg(s_msg_struct.msg_target,"\1ACTION tells "+s_msg_struct.nickname+" that the \">>\" command is only for: "+Dyrand+".\1");}
}



/*  Get Functions for Bot class */

std::string Bot::getNickname()
{return nickname;}

void Bot::getMsgStructs(serverMsgStruct& s_msg_struct_t, msgStruct& msg_struct_t)
{
    s_msg_struct_t = s_msg_struct;
    msg_struct_t = msg_struct;
}


