#include "ircMimic.hpp"
#include "Bot.hpp"

#include "iostream"

ircMimic::ircMimic(Bot* tempBot):

    bot(tempBot)
{}

void ircMimic::mimic()
{
    bot->getMsgStructs(s_msg_struct,msg_struct);

    //Goes through modifier vector and increments a modifer that matches
    for(unsigned int i(0); i < msg_struct.mods.size(); i++)
    {
        if(msg_struct.mods.at(i) == "-p")
        {user_f.pig_lat++;}
        else if(msg_struct.mods.at(i) == "+p")
        {user_f.pig_lat--;}
    }

    if(msg_struct.args.empty())
    {msg_struct.args.push_back(s_msg_struct.nick);}   //Saves not having to create a new condition

    for(unsigned int i(0); i < msg_struct.args.size(); i++)
    {
        if(msg_struct.args.at(i) == bot->getNick()){}//the bot will not mimic it self

        else if(std::find(nicks_mimic.begin(),nicks_mimic.end(),msg_struct.args.at(i)) != nicks_mimic.end())
        {bot->privmsg(s_msg_struct.msg_target,"Already mimicing what "+msg_struct.args.at(i)+" says.");}
        else
        {
            nicks_mimic.push_back(msg_struct.args.at(i));
            nick_map.emplace(msg_struct.args.at(i),user_f);
            bot->privmsg(s_msg_struct.msg_target,"Now mimicing what "+msg_struct.args.at(i)+" says.");
        }
    }

    mim_mods_reset();
}

//void ircMimic::mimic_description()
//{bot->privmsg(s_msg_struct.msg_target,"repeats everything a specific nickname says.");}
//void ircMimic::demimic_description()
//{bot->privmsg(s_msg_struct.msg_target,"stops mimic.");}


void ircMimic::mim_mods_reset()
{
    user_f.pig_lat = 0;
}

void ircMimic::demim_mods_reset()
{
    demim_all = 0;
}

void ircMimic::demimic()
{
    bot->getMsgStructs(s_msg_struct,msg_struct);

    for(unsigned int i(0); i < msg_struct.mods.size(); i++)
    {
        if(msg_struct.mods.at(i) == "-a")
        {demim_all++;}
    }

    if(demim_all)
    {
        if(nicks_mimic.empty())
        {bot->privmsg(s_msg_struct.msg_target,"No one to demimic.");}
        else
        {
            for(unsigned int i(0); i < nicks_mimic.size(); i++)
            {bot->privmsg(s_msg_struct.msg_target,"Not mimicing what "+nicks_mimic.at(i)+" says.");}
            nick_map.clear();
            nicks_mimic.clear();
        }
    }
    else
    {
        if(msg_struct.args.empty())
        {msg_struct.args.push_back(s_msg_struct.nick);}   //Saves not having to create a new condition

        for(unsigned int i(0); i < msg_struct.args.size(); i++)
        {
            auto iter = nicks_mimic.end();
            if((iter = std::find(nicks_mimic.begin(), nicks_mimic.end(),msg_struct.args[i])) != nicks_mimic.end())
            {
                bot->privmsg(s_msg_struct.msg_target,"Not mimicing what "+msg_struct.args[i]+" says.");
                nick_map.erase(msg_struct.args.at(i));
                nicks_mimic.erase(iter);
            }
        }
    }

    demim_mods_reset();
}

void ircMimic::mimic_handler()
{
    bot->getMsgStructs(s_msg_struct,msg_struct);
    if(!msg_struct.msg.empty())
    {
        if((msg_struct.msg.at(0) != msg_struct.ident) && (s_msg_struct.command == "PRIVMSG"))
        {
            if(std::find(nicks_mimic.begin(), nicks_mimic.end(),s_msg_struct.nick) != nicks_mimic.end())
            {
                if(nick_map.at(s_msg_struct.nick).pig_lat > 0)
                {
                    for(int i(0); i < nick_map.at(s_msg_struct.nick).pig_lat; i++)
                    {msg_struct.msg = pig_latin(msg_struct.msg);std::cout << msg_struct.msg << "\n";}
                }
                bot->privmsg(s_msg_struct.msg_target,msg_struct.msg);
            }
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
    for(size_t i(0); npos_reached == false; i++)
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
    for(unsigned int i(0);(i < (space_pos.size()-1)); i++)
    {
        temp_string = text.substr(space_pos.at(i)+1,((space_pos.at(i+1)-space_pos.at(i))-1));
        if(temp_string.size() != 0)
        {string_vect.push_back(temp_string);}
    }

    //Applies pig latin to divided string if it is only alpha numeric
    for(unsigned int i(0); i < string_vect.size(); i++)
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
    for(unsigned int i(0); i < string_vect.size(); i++)
    {
        temp_string.append(string_vect.at(i));
        temp_string.append(" ");
    }

    return temp_string;
}












