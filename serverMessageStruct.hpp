#ifndef SERVER_MESSAGE_STRUCT_HPP
#define SERVER_MESSAGE_STRUCT_HPP

#include <vector>
#include <string>

struct serverMessageStruct
{
    serverMessageStruct& operator=(serverMessageStruct);
    void reset();

    std::vector<int> space_pos;
    std::vector<std::string> args;

    std::string prefix   = "";
    std::string username = "";
    std::string command  = "";
    std::string channel  = "";
    std::string message  = "";
    std::string nickname = "";

    int prefix_end_pos = std::string::npos;
    int mes_start_pos  = std::string::npos;

    bool nick_flag = false; //used to note that channel name actually contains a nickname, e.g. /msg
};

struct messageStruct
{
    messageStruct& operator=(messageStruct);
    void reset();

    std::vector<int> space_pos;
    std::vector<std::string> args;
    std::vector<std::string> mods; //modifiers

    std::string message = "";
    std::string command = "";
    std::string postfix = "";

    int last_char_pos=0;

    const char ident = '>';
    const char minu  = '-';
    const char plu   = '+';
};


#endif // SERVER_MESSAGE_STRUCT_HPP
