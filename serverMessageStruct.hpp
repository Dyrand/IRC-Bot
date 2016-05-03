#ifndef SERVER_MESSAGE_STRUCT_HPP
#define SERVER_MESSAGE_STRUCT_HPP

#include <vector>
#include <string>

struct serverMsgStruct
{
    serverMsgStruct& operator=(serverMsgStruct);
    void reset();

    std::vector<std::string> args;
    std::vector<int> space_pos;

    std::string prefix;
    std::string user;
    std::string nickname;
    std::string command;
    std::string channel;
    std::string msg;
    std::string msg_target;

    size_t prefix_end_pos = std::string::npos;
    size_t msg_start_pos  = std::string::npos;
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

    size_t last_char_pos;

    char ident = '>';
    const char MINUS  = '-';
    const char PLUS   = '+';
};


#endif // SERVER_MESSAGE_STRUCT_HPP
