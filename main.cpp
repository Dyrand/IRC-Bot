#include <SFML/Network.hpp>

#include "BotClass.hpp"

int main()
{
    Bot bot;

    if(bot.connectToServer()==sf::Socket::Done)
    {
        if(bot.connectionRegistration()==sf::Socket::Done)
        {
            bot.join();
            bot.loop();
        }
    }
}
