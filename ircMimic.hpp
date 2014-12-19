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
        void mim_mods_reset();      //resets mimic modifiers to false
        //bool demim_check_conf_mods(); //checks if any demimic modifies conflict

        bool adjust_channel(); //replace channel with nickname if the channel var contains a nickname

    private:

        std::string pig_latin(std::string);

        Bot* bot;

        serverMsgStruct s_msg_struct;
        msgStruct         msg_struct;

        //Flags mimic
        struct flags_m
        {
            flags_m():
                pig_lat(false),
                desc(false),
                edit(false)
            {}
            bool pig_lat; //pig latin
            bool desc;    //decription
            bool edit;    //used to edit modifiers attached to currenct mimic
        }m_fs;

        //Flags demimic
        bool demimic_all = false;
        bool demim_desc = false;

        std::vector<std::string> nicks_mimic; //nicknames that are being mimiced
        std::map<std::string,flags_m> nick_map;

};

#endif // IRC_MIMIC_HPP
