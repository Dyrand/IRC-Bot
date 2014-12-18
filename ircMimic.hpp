#ifndef IRC_MIMIC_HPP
#define IRC_MIMIC_HPP

#include <map>

#include "serverMessageStruct.hpp"

class Bot;

//Includes demimic
class ircMimic
{
    public:
    ircMimic(Bot*);

    /*Core functions*/
    void mimic();
    void demimic();
    void mimic_handler();
    bool mimicing();

    void mimic_description();
    void demimic_description();

    bool mim_check_conf_mods(); //checks if any mimic modifies conflict
    void mim_mods_reset();      //resets modifiers to false
    //bool demim_check_conf_mods(); //checks if any demimic modifies conflict

    bool adjust_channel(); //replace channel with nickname if the channel var contains a nickname

    private:

    std::string pig_latin(std::string);

    Bot* bot;

    serverMessageStruct s_mes_struct;
    messageStruct         mes_struct;

    //Flags mimic
    struct flags_m
    {
        bool pig_lat; //pig latin
        bool desc;    //decription
        bool edit;    //used to edit modifiers attached to currenct mimic
    };

    flags_m m_fs; //mimic flags

    //Flags demimic
    bool demimic_all;
    bool demim_desc;

    std::vector<std::string> nicks_mimic; //nicknames that are being mimiced
    std::map<std::string,flags_m> nick_map;

};

#endif // IRC_MIMIC_HPP
