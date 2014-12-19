#include "ircMimic.hpp"
#include "Bot.hpp"

#include "iostream"

ircMimic::ircMimic(Bot* tempBot):

    demimic_all(false),

    bot(tempBot)
{}

void ircMimic::mimic()
{
    s_msg_struct = bot->s_msg_struct;
    msg_struct = bot->msg_struct;
    adjust_channel();

    //Goes through modifier vector and sets true any modifiers that are found
    for(int i(0); i < msg_struct.mods.size(); i++)
    {
        if(msg_struct.mods[i] == "-p")
        {m_fs.pig_lat = true;}
        else if(msg_struct.mods[i] == "-desc")
        {m_fs.desc = true;}
        else if(msg_struct.mods[i] == "-e")
        {m_fs.edit = true;}
    }

    //Exits function if there was an error with modifiers
    if(mim_check_conf_mods())
    {
        mim_mods_reset();
        return;
    }

    if(m_fs.desc)
    {
        mimic_description();
        mim_mods_reset();
        return;
    }

    if(m_fs.edit)
    {
        if(!msg_struct.args.empty())
        {
            if(std::find(nicks_mimic.begin(),nicks_mimic.end(),s_msg_struct.nick) != nicks_mimic.end())
            {
                nick_map.at(s_msg_struct.nick) = m_fs;
            }
        }
        else
        {bot->privmsg(s_msg_struct.channel,"-e can only be used with modifiers, not arguments.");}
        mim_mods_reset();
        return;
    }

    if(msg_struct.args.size() == 0)
    {msg_struct.args.push_back(s_msg_struct.nick);}   //Saves not having to create a new condition

    for(int i(0); i < msg_struct.args.size(); i++)
    {
        if(msg_struct.args.at(i) == bot->getnick()){}//dyramic won't mimic itself

        else if(std::find(nicks_mimic.begin(),nicks_mimic.end(),msg_struct.args.at(i)) != nicks_mimic.end())
        {bot->privmsg(s_msg_struct.channel,"Already mimicing what "+msg_struct.args.at(i)+" says.");}
        else
        {
            nicks_mimic.push_back(msg_struct.args.at(i));
            nick_map.emplace(msg_struct.args.at(i),m_fs);
            bot->privmsg(s_msg_struct.channel,"Now mimicing what "+msg_struct.args.at(i)+" says.");
        }
    }
    mim_mods_reset();
}

void ircMimic::mimic_description()
{
    bot->privmsg(s_msg_struct.channel,"repeats everything a specific nickname says.");
    m_fs.desc = false;
}
void ircMimic::demimic_description()
{bot->privmsg(s_msg_struct.channel,"stops mimic.");}

bool ircMimic::mim_check_conf_mods()
{

    if(m_fs.desc == true && (msg_struct.mods.size() > 1))
    {bot->privmsg(s_msg_struct.channel,"-desc can only be used by itself.");}
    else
    {return false;}

    return true;
}

void ircMimic::mim_mods_reset()
{
    m_fs.pig_lat = false;
    m_fs.desc    = false;
    m_fs.edit    = false;
}

void ircMimic::demimic()
{
    s_msg_struct = bot->s_msg_struct;
    msg_struct   = bot->msg_struct;
    adjust_channel();

    bool demimic_all = false;
    for(int i(0); i < msg_struct.mods.size(); i++)
    {
        if(msg_struct.mods[i] == "-a")
        {demimic_all=true;}
    }
    if(demimic_all == true)
    {
        if(nicks_mimic.empty())
        {bot->privmsg(s_msg_struct.channel,"No one to demimic.");}
        else
        {m_fs.pig_lat = false;
            for(int i(0); i < nicks_mimic.size(); i++)
            {bot->privmsg(s_msg_struct.channel,"Not mimicing what "+nicks_mimic[i]+" says.");}
            nick_map.clear();
            nicks_mimic.clear();
        }
    }
    else
    {
        if(msg_struct.args.size() == 0)
        {msg_struct.args.push_back(s_msg_struct.nick);}   //Saves not having to create a new condition

        for(int i(0); i < msg_struct.args.size(); i++)
        {
            auto iter = nicks_mimic.end();
            if((iter = std::find(nicks_mimic.begin(), nicks_mimic.end(),msg_struct.args[i])) != nicks_mimic.end())
            {
                bot->privmsg(s_msg_struct.channel,"Not mimicing what "+msg_struct.args[i]+" says.");
                nick_map.erase(msg_struct.args.at(i));
                nicks_mimic.erase(iter);
            }
        }
    }
}

void ircMimic::mimic_handler()
{
    s_msg_struct = bot->s_msg_struct;
    adjust_channel();

    if(s_msg_struct.msg.at(0)!=msg_struct.ident)
    {
        if(std::find(nicks_mimic.begin(), nicks_mimic.end(),s_msg_struct.nick) != nicks_mimic.end())
        {
            if(nick_map.at(s_msg_struct.nick).pig_lat)
            {s_msg_struct.msg = pig_latin(s_msg_struct.msg);}

            bot->privmsg(s_msg_struct.channel,s_msg_struct.msg);
        }
    }
}

bool ircMimic::mimicing()
{return (!nicks_mimic.empty());}

std::string ircMimic::pig_latin(std::string text)
{
    std::vector<int> space_pos;
    std::vector<std::string> string_vect;
    std::string temp_string;
    bool npos_reached = false;

    //Find spaces within text
    for(int i(0); npos_reached == false; i++)
    {
        i = text.find(" ",i);
        if(i != text.npos)
        {space_pos.push_back(i);}
        else
        {npos_reached = true;}
    }

    //Divide text based on spaces
    space_pos.emplace(space_pos.begin(),-1); //Padding for beginning
    space_pos.emplace_back(text.size());     //Padding for end
    for(int i(0);(i < (space_pos.size()-1)); i++)
    {
        temp_string = text.substr(space_pos[i]+1,((space_pos[i+1]-space_pos[i])-1));
        if(temp_string.size() != 0)
        {string_vect.push_back(temp_string);}
    }

    //Applies pig latin to divide string if it is only alpha numeric
    for(int i(0); i < string_vect.size(); i++)
    {
        if(string_vect.at(i).find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") == string_vect.at(i).npos)
        {
            if(string_vect.at(i).size() != 0)
            {
                string_vect.at(i)+=string_vect.at(i).at(0);
                string_vect.at(i).erase(string_vect.at(i).begin());
                string_vect.at(i).append("ay");
            }
        }
    }

    //Combines the divided strings back into one
    temp_string.assign("");
    for(int i(0); i < string_vect.size(); i++)
    {
        temp_string.append(string_vect.at(i));
        temp_string.append(" ");
    }

    return temp_string;
}

bool ircMimic::adjust_channel()
{
    if(s_msg_struct.nick_flag)
    {
        s_msg_struct.channel = s_msg_struct.nick;
        return true;
    }
    return false;
}












