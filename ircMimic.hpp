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

        //void mimic_description();
        //void demimic_description();

        void mim_mods_reset();      //resets mimic modifiers to 0
        void demim_mods_reset();    //resets demimic modifiers to 0
        //bool demim_check_conf_mods(); //checks if any demimic modifies conflict

    private:

        std::string pig_latin(std::string);

        Bot* bot;

        serverMsgStruct s_msg_struct;
        msgStruct         msg_struct;

        //Flags mimic
        struct user_flags
        {
            int pig_lat = 0; //pig latin
        }user_f;

        //Flags demimic
        int demim_all = 0;


        std::vector<std::string> nicknames_mimic; //nicknames that are being mimiced
        std::map<std::string,user_flags> nickname_map;

};

#endif // IRC_MIMIC_HPP
