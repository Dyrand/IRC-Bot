#include "ircMimic.hpp"
#include "Bot.hpp"

#include "iostream"

ircMimic::ircMimic(Bot* tempBot):

    demimic_all(false),

    bot(tempBot)
{}

void ircMimic::mimic()
{
    s_mes_struct = bot->s_mes_struct;
    mes_struct = bot->mes_struct;
    adjust_channel();

    //Goes through modifier vector and sets true any modifiers that are found
    for(int i(0); i < mes_struct.mods.size(); i++)
    {
        if(mes_struct.mods[i] == "-p")
        {m_fs.pig_lat = true;}
        else if(mes_struct.mods[i] == "-desc")
        {m_fs.desc = true;}
        else if(mes_struct.mods[i] == "-e")
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
        if(mes_struct.args.size() == 0)
        {
            if(std::find(nicks_mimic.begin(),nicks_mimic.end(),s_mes_struct.nick) != nicks_mimic.end())
            {
                nick_map.at(s_mes_struct.nick) = m_fs;
            }
        }
        else
        {bot->privmsg(s_mes_struct.channel,"-e can only be used with modifiers, not arguments.");}
        mim_mods_reset();
        return;
    }

    if(mes_struct.args.size() == 0)
    {mes_struct.args.push_back(s_mes_struct.nick);}   //Saves not having to create a new condition

    for(int i(0); i < mes_struct.args.size(); i++)
    {
        if(mes_struct.args.at(i) == bot->getnick()){}//dyramic won't mimic itself

        else if(std::find(nicks_mimic.begin(),nicks_mimic.end(),mes_struct.args.at(i)) != nicks_mimic.end())
        {bot->privmsg(s_mes_struct.channel,"Already mimicing what "+mes_struct.args.at(i)+" says.");}
        else
        {
            nicks_mimic.push_back(mes_struct.args.at(i));
            nick_map.emplace(mes_struct.args.at(i),m_fs);
            bot->privmsg(s_mes_struct.channel,"Now mimicing what "+mes_struct.args.at(i)+" says.");
        }
    }
    mim_mods_reset();
}

void ircMimic::mimic_description()
{
    bot->privmsg(s_mes_struct.channel,"repeats everything a specific nickname says.");
    m_fs.desc = false;
}
void ircMimic::demimic_description()
{bot->privmsg(s_mes_struct.channel,"stops mimic.");}

bool ircMimic::mim_check_conf_mods()
{

    if(m_fs.desc == true && (mes_struct.mods.size() > 1))
    {bot->privmsg(s_mes_struct.channel,"-desc can only be used by itself.");}
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
    s_mes_struct = bot->s_mes_struct;
    mes_struct   = bot->mes_struct;
    adjust_channel();

    bool demimic_all = false;
    for(int i(0); i < mes_struct.mods.size(); i++)
    {
        if(mes_struct.mods[i] == "-a")
        {demimic_all=true;}
    }
    if(demimic_all == true)
    {
        if(nicks_mimic.empty())
        {bot->privmsg(s_mes_struct.channel,"No one to demimic.");}
        else
        {m_fs.pig_lat = false;
            for(int i(0); i < nicks_mimic.size(); i++)
            {bot->privmsg(s_mes_struct.channel,"Not mimicing what "+nicks_mimic[i]+" says.");}
            nick_map.clear();
            nicks_mimic.clear();
        }
    }
    else
    {
        if(mes_struct.args.size() == 0)
        {mes_struct.args.push_back(s_mes_struct.nick);}   //Saves not having to create a new condition

        for(int i(0); i < mes_struct.args.size(); i++)
        {
            auto iter = nicks_mimic.end();
            if((iter = std::find(nicks_mimic.begin(), nicks_mimic.end(),mes_struct.args[i])) != nicks_mimic.end())
            {
                bot->privmsg(s_mes_struct.channel,"Not mimicing what "+mes_struct.args[i]+" says.");
                nick_map.erase(mes_struct.args.at(i));
                nicks_mimic.erase(iter);
            }
        }
    }
}

void ircMimic::mimic_handler()
{
    s_mes_struct = bot->s_mes_struct;
    adjust_channel();

    if(s_mes_struct.message.at(0)!=mes_struct.ident)
    {
        if(std::find(nicks_mimic.begin(), nicks_mimic.end(),s_mes_struct.nick) != nicks_mimic.end())
        {
            if(nick_map.at(s_mes_struct.nick).pig_lat)
            {s_mes_struct.message = pig_latin(s_mes_struct.message);}

            bot->privmsg(s_mes_struct.channel,s_mes_struct.message);
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
    if(s_mes_struct.nick_flag)
    {
        s_mes_struct.channel = s_mes_struct.nick;
        return true;
    }
    return false;
}












