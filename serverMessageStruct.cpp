#include "serverMessageStruct.hpp"

serverMsgStruct& serverMsgStruct::operator=(serverMsgStruct temp)
{
    args       = temp.args;
    space_pos  = temp.space_pos;
    prefix     = temp.prefix;
    user       = temp.user;
    nickname   = temp.nickname;
    command    = temp.command;
    channel    = temp.channel;
    msg        = temp.msg;
    msg_target = temp.msg_target;
    prefix_end_pos = temp.prefix_end_pos;
    msg_start_pos  = temp.msg_start_pos;

    return *this;
}

void serverMsgStruct::reset()
{
    args.clear();
    space_pos.clear();
    prefix.clear();
    user.clear();
    nickname.clear();
    command.clear();
    channel.clear();
    msg.clear();
    msg_target.clear();
    prefix_end_pos = std::string::npos;
    msg_start_pos  = std::string::npos;
}

msgStruct& msgStruct::operator=(msgStruct temp)
{
    args      = temp.args;
    mods      = temp.mods;
    space_pos = temp.space_pos;
    msg       = temp.msg;
    command   = temp.command;
    postfix   = temp.postfix;
    last_char_pos = temp.last_char_pos;
    return *this;
}

void msgStruct::reset()
{
    args.clear();
    mods.clear();
    space_pos.clear();
    msg.clear();
    command.clear();
    postfix.clear();
    last_char_pos = std::string::npos;
}
