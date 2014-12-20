#ifndef SERVER_MESSAGE_STRUCT_HPP
#define SERVER_MESSAGE_STRUCT_HPP

#include <vector>
#include <string>

struct serverMsgStruct
{
    serverMsgStruct& operator=(serverMsgStruct);
    void reset();
    void adjust_channel(); //Changes s_msg_struct msg_target from channel to nick if nick_flag is true

    std::vector<std::string> args;
    std::vector<int> space_pos;

    std::string prefix;
    std::string user;
    std::string nick;
    std::string command;
    std::string channel;
    std::string msg;
    std::string msg_target;

    size_t prefix_end_pos = std::string::npos;
    size_t msg_start_pos  = std::string::npos;

    bool nick_flag = false; //used to note that channel name actually contains a nickname, e.g. /msg
};

struct msgStruct
{
    msgStruct& operator=(msgStruct);
    void reset();

    std::vector<std::string> args; //arguments
    std::vector<std::string> mods; //modifiers
    std::vector<int> space_pos;

    std::string msg;
    std::string command;
    std::string postfix;

    size_t last_char_pos = std::string::npos;

    char ident = '>';
    const char minu  = '-';
    const char plu   = '+';
};


#endif // SERVER_MESSAGE_STRUCT_HPP
