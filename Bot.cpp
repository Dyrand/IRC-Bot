#include <iostream>
#include <string>
#include <fstream>

#include <SFML/Network.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/config.hpp>


#include "TcpSocket.hpp"
#include "Bot.hpp"
#include "serverMessageStruct.hpp"

Bot::Bot():
  mimic_o(this),
  config_file("config/config.txt"),
  stay_connected(true)
{ configure(); }

sf::Socket::Status Bot::connectToServer(std::string serverName_t, unsigned short port_t)
{
    target_server = serverName_t;
    target_port = port_t;
    return connectToServer();
}

sf::Socket::Status Bot::connectToServer()
{
    return socket.connect(target_server,target_port,max_wait_time);
}

void Bot::registerConnection(std::string connection_password_t, std::string nickname_t, std::string user_t, std::string realname_t, std::string mode_t = "0")
{
    connection_password = connection_password_t;
    nickname = nickname_t;
    user = user_t;
    realname = realname_t;
    mode = mode_t;
    registerConnection();
}

void Bot::registerConnection()
{
    send("PASS "+connection_password);
    send("NICK "+nickname);
    send("USER "+user+" "+mode+" * :"+realname);
}

sf::Socket::Status Bot::send(std::string sendMessage_t)
{
    sendMessage = sendMessage_t;
    sendMessage.append("\r\n");
    return socket.send(sendMessage.c_str(),sendMessage.size());
}

/*
   Using A TCPsocket, divides a message based on "\r\n" position.
   messages without a "\r\n" at the end are regarded as
   partial strings and are saved for recombining.
*/
void Bot::receive()
{
    bool isPartialString = false;

    socket.receive(text_received.data(),text_received.size(),bytes_rec);
    string_received.assign(text_received.begin(),text_received.begin()+bytes_rec);

    //Appends one partial strings to another
    rn_pos = string_received.find("\r\n");
    if(isPartialString && (rn_pos != string_received.npos))
    {
        temp_string = string_received.substr(0,rn_pos+2);
        parsable_strings.emplace_back(partial_string+temp_string);
        string_received.erase(0,rn_pos+2);
        partial_string.clear();
        isPartialString = false;
    }

    //Split strings on "\r\n" positions and removes characters until there is no "\r\n" left
    while((rn_pos = string_received.find("\r\n")) != string_received.npos)
    {
        temp_string = string_received.substr(0,rn_pos+2);
        parsable_strings.emplace_back(temp_string);
        string_received.erase(0,rn_pos+2);
    }

    //If there is no "\r\n" left, but there are charcters still left in the string, it is a partial string
    if(!string_received.empty())
    {
        partial_string.assign(string_received);
        string_received.clear();
        isPartialString = true;
    }
}

void Bot::privmsg(std::string channel_t, std::string sendMessage_t)
{
    target_channel = channel_t;
    privmsg(sendMessage_t);
}

void Bot::privmsg(std::string sendMessage_t)
{
    sendMessage = sendMessage_t;
    send("PRIVMSG "+target_channel+" :"+sendMessage);
}

void Bot::join(std::string channel_t)
{
    target_channel = channel_t;
    join();
}

void Bot::join()
{
    send("JOIN "+target_channel);
}

void Bot::part(std::string channel_t, std::string sendMessage_t)
{
    channel = channel_t;
    sendMessage = sendMessage_t;
    send("PART "+channel+" :"+sendMessage);
}

void Bot::part(std::string channel_t)
{
    channel = channel_t;
    send("PART "+channel);
}

void Bot::quit(std::string sendMessage_t)
{
    sendMessage = sendMessage_t;
    send("QUIT :"+sendMessage);
}

void Bot::quit()
{
    send("QUIT :"+quit_message);
}


void Bot::parseServerMsg()
{
//<-----------prefix-----------> <command> <channel> :<message>
//<nick|servername>!<username>@ <command> <channel> :<message>
//:Dyrand!~Dyrand@unaffiliated/dyrand PRIVMSG #botdever :9

//<nick|servername>!<username>@ <command> :<message>
//:Dyrand!~Dyrand@unaffiliated/dyrand QUIT :Client Closed
//:QKiller!~usandbots@unaffiliated/usandfriends JOIN #botdever

//:sendak.freenode.net 004 Dyramic sendak.freenode.net ircd-seven-1.1.3

//PING :<pong>

//ERROR :Closing Link: 127.0.0.1 (Connection timed out)
//HYPOTHETICALMESSAGE  /*no space and no : or !

    std::cout << string_received;

    rn_pos = string_received.find("\r\n");
    if(rn_pos == std::string::npos)
    {std::cout << "Receive string does not contain \"\\r\\n\".";}

    //Gets the prefix if there is one
    if(string_received.front() == ':')
    {
      s_msg_struct.prefix_end_pos = string_received.find(' ');
      if(s_msg_struct.prefix_end_pos != std::string::npos)
      { s_msg_struct.prefix = string_received.substr(1,s_msg_struct.prefix_end_pos); }
      else
      { std::cout << "Receive string does not contain a space.\n"; }
    }
    else
    {std::cout << "Receive string does not start with a ':'.\n";}

    //Gets the message if there is one
    s_msg_struct.msg_start_pos = string_received.find(" :");
    if(s_msg_struct.msg_start_pos != std::string::npos)
    { s_msg_struct.msg = string_received.substr(s_msg_struct.msg_start_pos+2,(rn_pos-s_msg_struct.msg_start_pos)-2); }
    else
    {
        std::cout << "Receive string does not contain a message.\n";
        //padding for correctly getting the last block of text
        string_received.insert(rn_pos," ");
        s_msg_struct.msg_start_pos = rn_pos;
    }

    //Find all the spaces between prefix_end_pos and mes_start_pos
    space_pos = string_received.find(' ',++space_pos);
    while(space_pos < s_msg_struct.msg_start_pos)
    {
      s_msg_struct.space_pos.emplace_back(space_pos);
      space_pos = string_received.find(' ',++space_pos);
    }

    //Padding for collecting last string in string_received apart from message
    s_msg_struct.space_pos.emplace_back(s_msg_struct.msg_start_pos);

    //Gets the command in the message
    s_msg_struct.command = string_received.substr(s_msg_struct.prefix_end_pos+1,(s_msg_struct.space_pos.at(0)-s_msg_struct.prefix_end_pos)-1);

    //Implies the 2nd piece after the message is a channel or nickname, both go to channel var though
    if(s_msg_struct.space_pos.size() >= 2)
    { s_msg_struct.channel = string_received.substr(s_msg_struct.space_pos.at(0)+1,(s_msg_struct.space_pos.at(1)-s_msg_struct.space_pos.at(0))-1); }

    //Parse the prefix into nickname and username if possible
    excl_pos = s_msg_struct.prefix.find('!');
    if(excl_pos != std::string::npos)
    { s_msg_struct.nickname = s_msg_struct.prefix.substr(0,excl_pos); }

    at_pos = s_msg_struct.prefix.find('@');
    if(at_pos != std::string::npos)
    {
        if(s_msg_struct.prefix.at(excl_pos+1) == '~')
        { s_msg_struct.user = s_msg_struct.prefix.substr(excl_pos+2,(at_pos-excl_pos)-2); }
        else
        { s_msg_struct.user = s_msg_struct.prefix.substr(excl_pos+1,(at_pos-excl_pos)-1); }
    }

    //The rest of the stuff left between spaces is put into a string vector
    for(unsigned int i(1); i < (s_msg_struct.space_pos.size()-1); ++i)
    { s_msg_struct.args.emplace_back(string_received.substr(s_msg_struct.space_pos.at(i)+1,s_msg_struct.space_pos.at(i+1)-s_msg_struct.space_pos.at(i))); }

    //Check what the msg_target should be
    if(s_msg_struct.channel == nickname)
    { s_msg_struct.msg_target = nickname; }
    else
    { s_msg_struct.msg_target = s_msg_struct.channel; }
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
    if(msg_struct.msg.front() != msg_struct.ident)
    {return;}

    //Find the spaces within the message
    space_pos = msg_struct.msg.find(' ');
    while(space_pos != std::string::npos)
    {
      msg_struct.space_pos.emplace_back(space_pos);
      space_pos = msg_struct.msg.find(' ',++space_pos);
    }

    //Adds a padding space for the end of the message for substr
    msg_struct.space_pos.emplace_back(msg_struct.space_pos.size());

    //Simple substr to get command
    msg_struct.command = msg_struct.msg.substr(1,(msg_struct.space_pos.at(0)-1));

    bool modifier_present = true;
    unsigned int i(0);
    //Parsing modifiers
    for(; (i < (msg_struct.space_pos.size()-1)) && modifier_present; ++i)
    {
        temp_string = msg_struct.msg.substr(msg_struct.space_pos.at(i)+1,((msg_struct.space_pos.at(i+1)-msg_struct.space_pos.at(i)))-1);

        if(!temp_string.empty())
        {
            if(temp_string.front()==msg_struct.MINUS || temp_string.front()==msg_struct.PLUS)
            {msg_struct.mods.emplace_back(temp_string);}
            else
            {
                modifier_present = false;
                --i; //So argument parser can get the arguments instead
            }
        }
    }
    //Parsing arguments
    for(; i < (msg_struct.space_pos.size()-1); ++i)
    {
        temp_string = msg_struct.msg.substr(msg_struct.space_pos.at(i)+1,(msg_struct.space_pos.at(i+1)-msg_struct.space_pos.at(i))-1);

        if(!temp_string.empty())
        { msg_struct.args.emplace_back(temp_string); }
    }
    msg_struct.space_pos.pop_back(); //Remove the space padding

    //Gets the postfix if there is one
    string_length = msg_struct.command.size();
    if(string_length != msg_struct.last_char_pos)
    { msg_struct.postfix = msg_struct.msg.substr(string_length+2,(msg_struct.last_char_pos-string_length)-1); }
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
    {send("PONG "+s_msg_struct.msg);}
}



void Bot::resetVars()
{
    s_msg_struct.reset();
    msg_struct.reset();
}


void Bot::loop()
{
    while(stay_connected)
    {
        receive();
        for(int i(0); i < parsable_strings.size(); ++i)
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
    }
    quit();
    socket.disconnect();
}

void Bot::discon()
{
    if(s_msg_struct.nickname == Dyrand)
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

void Bot::configure()
{
  using namespace boost::program_options;

  if(!exists(config_file))
  {genConfig();}

  options_description desc("Allowed options");

  /*Boost options_description class uses a special proxy return on call to add_options*/
  /* The following is syntactically correct */
  desc.add_options()
    ("max_wait_time", value<int>(), "The maximum time allotted for connecting to a server.")
    ("connection_password", value<std::string>(), "The password assigned to a connection established with the sever.")
    ("nickname", value<std::string>(), "The nickname for the bot.")
    ("user", value<std::string>(), "The username for the bot.")
    ("realname", value<std::string>(), "The realname for the bot.")
    ("mode", value<std::string>(), "The mode can be 8 which corresponds to 'i' (invisible) or 0 for no mode.")
    ("quit_message", value<std::string>(), "The message the bot displays when it sends the QUIT command.")
    ("target_port", value<unsigned short>(), "The port the bot will use to connect to the server.")
    ("target_server", value<std::string>(), "The server the bot will initially connecto to.")
    ("target_channel", value<std::string>(), "The channel the bot will initially connect to, \'%\' are substituted for \'#\'s.")
  ;

  std::ifstream in_file(config_file.string());

  variables_map vm;
  store(parse_config_file(in_file,desc), vm);
  notify(vm);

  /*Set each variable to the values parsed from config*/
  max_wait_time = vm["max_wait_time"].as<int>();
  connection_password = vm["connection_password"].as<std::string>();
  nickname = vm["nickname"].as<std::string>();
  user = vm["user"].as<std::string>();
  realname = vm["realname"].as<std::string>();
  mode = vm["mode"].as<std::string>();
  quit_message = vm["quit_message"].as<std::string>();
  target_port = vm["target_port"].as<unsigned short>();
  target_server = vm["target_server"].as<std::string>();
  target_channel = vm["target_channel"].as<std::string>();

  /*Replace first sequence of '%'s with '#'s*/
  auto index = target_channel.find_first_not_of('%');
  target_channel.replace(0,index,index,'#');

}

void Bot::genConfig()
{
  if(!exists(*config_file.begin()))
  {create_directory(*config_file.begin());}

  std::ofstream out_file(config_file.string());

  out_file << "max_wait_time=5" << "\n";
  out_file << "connection_password=password" << "\n";
  out_file << "nickname=DyrBot" << "\n";
  out_file << "user=DyrBot" << "\n";
  out_file << "realname=DyrBot" << "\n";
  out_file << "mode=0" << "\n";
  out_file << "quit_message=gotta go fast!" << "\n";
  out_file << "target_port=8001" << "\n";
  out_file << "target_server=irc.freenode.com" << "\n";
  out_file << "target_channel=%botdever" << "\n";

  out_file.close();
}
