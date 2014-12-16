#include "serverMessageStruct.hpp"

serverMessageStruct& serverMessageStruct::operator=(serverMessageStruct temp)
{
    space_pos = temp.space_pos;
    args      = temp.args;
    prefix    = temp.prefix;
    username  = temp.username;
    command   = temp.command;
    channel   = temp.channel;
    message   = temp.message;
    nickname  = temp.nickname;
    nick_flag = temp.nick_flag;
}

void serverMessageStruct::reset()
{
    space_pos.clear();
    args.clear();
    prefix.clear();
    username.clear();
    command.clear();
    channel.clear();
    message.clear();
    nickname.clear();

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
