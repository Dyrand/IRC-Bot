#include "serverMessageStruct.hpp"

serverMessageStruct& serverMessageStruct::operator=(serverMessageStruct temp)
{
    space_pos = temp.space_pos;
    args      = temp.args;
    prefix    = temp.prefix;
    user  = temp.user;
    command   = temp.command;
    channel   = temp.channel;
    message   = temp.message;
    nick  = temp.nick;
    prefix_end_pos = temp.prefix_end_pos;
    mes_start_pos  = temp.mes_start_pos;
    nick_flag = temp.nick_flag;

    return *this;
}

void serverMessageStruct::reset()
{
    space_pos.clear();
    args.clear();
    prefix.clear();
    user.clear();
    command.clear();
    channel.clear();
    message.clear();
    nick.clear();

    prefix_end_pos = std::string::npos;
    mes_start_pos  = std::string::npos;

    nick_flag = false;
}

messageStruct& messageStruct::operator=(messageStruct temp)
{
    space_pos = temp.space_pos;
    args      = temp.args;
    mods      = temp.mods;
    message   = temp.message;
    command   = temp.command;
    postfix   = temp.postfix;

    last_char_pos = temp.last_char_pos;
    return *this;
}

void messageStruct::reset()
{
    space_pos.clear();
    args.clear();
    mods.clear();
    message.clear();
    command.clear();
    postfix.clear();

    last_char_pos=0;
}
