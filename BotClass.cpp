#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdlib>

#include <SFML/Network.hpp>

#include "BotClass.hpp"
#include "serverMessageStruct.hpp"

    const std::string ircBotClass::PONG("PONG");
    const std::string ircBotClass::PING("PING");
    const std::string ircBotClass::PASS("PASS");
    const std::string ircBotClass::PRIVMSG("PRIVMSG");
    const std::string ircBotClass::USER("USER");
    const std::string ircBotClass::NICK("NICK");
    const std::string ircBotClass::JOIN("JOIN");
    const std::string ircBotClass::QUIT("QUIT");

    const std::string ircBotClass::unused ("*");
    const std::string ircBotClass::s(" ");
    const std::string ircBotClass::c(":");
    const std::string ircBotClass::rn("\r\n");

    const std::string ircBotClass::Dyrand("Dyrand");

ircBotClass::ircBotClass(): mimic_o(this)
{
    formated_text = "";
    receive_string = "";
    stay_connected = true;
    partial_string = "";

    std::string userInput;
    std::cout << "Use default settings for the bot or custom?\n";
    std::getline(std::cin,userInput);
    if((userInput=="d") || (userInput=="D") || (userInput=="default"))
    {
        connection_password = "op10";
        nickname = "Dyramic";
        username = "dyramic";
        realname = "dyramic";
        mode     = "0";

        target_channel  = "#botdever";
        server   = "irc.freenode.com";
        port     = 8001;
    }
    else
    {
        connection_password = "op10";
        nickname = "Dyramic";
        username = "dyramic";
        realname = "dyramic";
        mode     = "0";

        target_channel  = "#botdever";
        server   = "irc.freenode.com";
        port     = 8001;
    }
}

int ircBotClass::connectToServer(std::string server, int port)
{return statusSwitch(status = socket.connect(server,port));}

int ircBotClass::connectToServer()
{return statusSwitch(status = socket.connect(server,port));}


int ircBotClass::connectionRegistration()
{
    send(PASS+s+connection_password+rn);
    send(NICK+s+nickname+rn);
    send(USER+s+username+s+mode+s+unused+s+c+realname+rn);
    return status;
}

int ircBotClass::statusSwitch(int status)
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


void ircBotClass::send(std::string formated_text)
{
    std::cout << formated_text;
    status = socket.send(formated_text.c_str(),formated_text.size());
}

//Using A TCPsocket, divide a message based on "\r\n" position, messages with out a "\r\n" at the end is regarded as
//a partial string and saved for combining
void ircBotClass::receive()
{
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


void ircBotClass::join(std::string target_channel)
{send(JOIN+s+target_channel+rn);}
void ircBotClass::join()
{send(JOIN+s+target_channel+rn);}

void ircBotClass::privmsg(std::string channel, std::string message)
{send(PRIVMSG+s+channel+s+c+message+rn);}


void ircBotClass::parserOfServerMessages()
{
//<-----------prefix-----------> <command> <channel> :<message>
//<nick|servername>!<username>@ <command> <channel> :<message>
//jamie!jamie@127.0.0.1 PRIVMSG #Node.js :Hello! I was just  browsing for Node.js help, found this channel."
//:Dyrand!~Dyrand@unaffiliated/dyrand PRIVMSG #botdever :9

//<nick|servername>!<username>@ <command> :<message>
//:Dyrand!~Dyrand@unaffiliated/dyrand QUIT :Client Closed
//:QKiller!~usandbots@unaffiliated/usandfriends JOIN #botdever

//:sendak.freenode.net 004 Dyramic sendak.freenode.net ircd-seven-1.1.3 DOQRSZaghilopswz CFILMPQSbcefgijklmnopqrstvz bkloveqjfI

//PING :<pong>

//ERROR :Closing Link: 127.0.0.1 (Connection timed out)
//HYPOTHETICALMESSAGE  /*no space and no : or !

    //Check if receive string is not just \r\n
    if(receive_string.size() < 2)
    {return;}


    //Check if string has at least one spaces
    if(receive_string.find(s) == receive_string.npos)
    {
        s_mes_struct.prefix = receive_string;
        return;
    }


    //Check if string begins with ':'
    if(receive_string.at(0) != ':')
    {
        space_pos = receive_string.find(s);
        rn_pos = receive_string.find(rn);
        s_mes_struct.prefix = receive_string.substr(0,space_pos);
        s_mes_struct.message = receive_string.substr(space_pos+1,(rn_pos-space_pos)-1); //substr includes colon
        return;
    }


    //strip the ':'
    else
    {receive_string.erase(receive_string.begin());}


    //Check if the string has at least one colon after a space ,if not add "colon " :" just before "\r\n"
    no_mes = false;
    if(receive_string.find(" :") == receive_string.npos)
    {
        receive_string.insert(receive_string.find(rn)," :");
        no_mes = true;
    }


    //Find all space in receive_string
    npos_reached = false;
    mes_reached  = false;
    for(int i(0); (npos_reached == false) && (mes_reached == false); i++)
    {
        i = receive_string.find(s,i);

        if(i != s_mes_struct.message.npos)
        {
            s_mes_struct.space_pos.push_back(i);
            if(receive_string.at(i+1) == receive_string.at(col_pos = receive_string.find(c,i))) //Check if the character after the current space is a colon
            {
                s_mes_struct.message = receive_string.substr(col_pos+1,(receive_string.find(rn,i)-col_pos)-1);
                mes_reached = true;
            }
        }
        else
        {npos_reached = true;}
    }

    //Use spaces found to divide message into parts
    s_mes_struct.space_pos.emplace(s_mes_struct.space_pos.begin(),-1); //Added to substr the first piece of text in the loop

    for(int i(0); i < s_mes_struct.space_pos.size()-1; i++)
    {block_of_text.emplace_back(receive_string.substr(s_mes_struct.space_pos.at(i)+1,((s_mes_struct.space_pos.at(i+1)-s_mes_struct.space_pos.at(i))-1)));}

    s_mes_struct.space_pos.erase(s_mes_struct.space_pos.begin()); //Used to restore vector back to it's original content


    //Place content into it's correct place
    if(block_of_text.size() > 0){s_mes_struct.prefix = block_of_text.at(0);}
    if(block_of_text.size() > 1){s_mes_struct.command = block_of_text.at(1);}
    if(block_of_text.size() > 2)
    {
        s_mes_struct.channel = block_of_text.at(2);
        if(s_mes_struct.channel.find('#') == s_mes_struct.channel.npos) //Check if the channel string is actually a nickname
        {s_mes_struct.nick_flag = true;}
    }

    //Parse prefix if possible
    excl_pos = s_mes_struct.prefix.find("!");
    at_pos = s_mes_struct.prefix.find("@");
    if((excl_pos != s_mes_struct.prefix.npos) && (at_pos != s_mes_struct.prefix.npos))
    {
        {s_mes_struct.nickname = s_mes_struct.prefix.substr(0,excl_pos);}
        if(s_mes_struct.prefix.at(excl_pos+1) == '~')
        {s_mes_struct.username = s_mes_struct.prefix.substr(excl_pos+2,((at_pos-excl_pos)-2));}
        else
        {s_mes_struct.username = s_mes_struct.prefix.substr(excl_pos+1,((at_pos-excl_pos)-1));}
    }

    if(no_mes == true)
    {
        //strip colon at end, added earlier for padding
        receive_string.erase(receive_string.begin()+receive_string.rfind(c));
        for(int i(2); i < (s_mes_struct.space_pos.size()-1) ; i++)
        {s_mes_struct.args.emplace_back(receive_string.substr(s_mes_struct.space_pos.at(i)+1,((s_mes_struct.space_pos.at(i+1)-s_mes_struct.space_pos.at(i))-1)));}
    }

    //std::cout << s_mes_struct.prefix << rn;
    //std::cout << "command: " << s_mes_struct.command << rn;
    //std::cout << "channel: " << s_mes_struct.channel << rn;
    //std::cout << "message: " << s_mes_struct.message << rn+rn;

    //for(int i(0); i < s_mes_struct.args.size(); i++)
    //{std::cout << "Argument"<< i << ": " << s_mes_struct.args.at(i) << rn;}
}


void ircBotClass::postHandler()
{
    if(mimic_o.mimicing())
    {mimic_o.mimic_handler();}
}


void ircBotClass::parserOfMessages()
{
    if(s_mes_struct.message.size() == 0)
    {return;}

    mes_struct.message = s_mes_struct.message;
    mes_struct.last_char_pos = mes_struct.message.length()-1;

    if(mes_struct.message.at(0) != mes_struct.ident)
    {return;}

    //Find the spaces within the message
    bool npos_reached = false;
    for(int i(0); npos_reached == false; i++)
    {
        i = mes_struct.message.find(s,i);
        if(i != mes_struct.message.npos)
        {mes_struct.space_pos.push_back(i);}
        else
        {npos_reached = true;}
    }

    //Space for the end, makes the following statements work
    mes_struct.space_pos.push_back(mes_struct.space_pos.size());

    //Simple substr to get command
    mes_struct.command = mes_struct.message.substr(1,(mes_struct.space_pos[0]-1));

    bool modifier_present = true;
    int i(0);
    //Parsing modifiers
    for(; (i < (mes_struct.space_pos.size()-1)) & modifier_present; i++)
    {
        temp_string = mes_struct.message.substr(mes_struct.space_pos[i]+1,((mes_struct.space_pos[i+1]-mes_struct.space_pos[i])-1));

        if(temp_string.size() != 0)
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
        temp_string = mes_struct.message.substr(mes_struct.space_pos[i]+1,((mes_struct.space_pos[i+1]-mes_struct.space_pos[i])-1));

        if(temp_string.size() != 0)
        {mes_struct.args.push_back(temp_string);}
    }
    mes_struct.space_pos.pop_back(); //Remove the space padding
}


void ircBotClass::checkCommands()
{
    if(mes_struct.command == "mimic" && s_mes_struct.command == PRIVMSG)
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

void ircBotClass::serverResponse()
{
    if(s_mes_struct.prefix==PING)
    {send(PONG+s+s_mes_struct.message+rn);}
}



void ircBotClass::resetVars()
{
    s_mes_struct.reset();
    mes_struct.reset();
    block_of_text.clear();
}


void ircBotClass::loop()
{
    do
    {
        receive();
        for(int i(0); i < parsable_strings.size(); i++)
        {
            receive_string = parsable_strings.at(i);
            if(receive_string.size() != 0)
            {
                parserOfServerMessages();
                if(s_mes_struct.channel.size() != 0)
                {
                    parserOfMessages();
                    if(mes_struct.command.size() != 0)
                    {checkCommands();}
                }
                else
                {serverResponse();}
            }
            postHandler();
            resetVars();
        }parsable_strings.clear();
    }while(stay_connected==true);
    send(QUIT+" :Ever want to look more like?"+rn);
    socket.disconnect();
}

void ircBotClass::disconnect()
{
    if(s_mes_struct.nickname == Dyrand)
    {stay_connected = false;}
}

void ircBotClass::rawInput()
{
    if((s_mes_struct.nickname == Dyrand) && (mes_struct.message.size() > 2))
    {
    mes_struct.postfix = mes_struct.message.substr(3,mes_struct.last_char_pos-1);
    send(mes_struct.postfix+rn);
    }
    else
    {privmsg(s_mes_struct.channel,"\1ACTION tells "+s_mes_struct.nickname+" that the \">>\" command is only for: "+Dyrand+".\1");}
}

std::string ircBotClass::getNickname()
{return nickname;}


