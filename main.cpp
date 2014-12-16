#include <SFML/Network.hpp>

#include "BotClass.hpp"

int main()
{
    ircBotClass bot;

    if(bot.connectToServer()==sf::Socket::Done)
    {
        if(bot.connectionRegistration()==sf::Socket::Done)
        {
            bot.join();
            bot.loop();
        }
    }
}
