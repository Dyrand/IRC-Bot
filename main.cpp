#include <SFML/Network.hpp>

#include "Bot.hpp"

int main()
{
    Bot bot;

    if(bot.connectToServer()==sf::Socket::Done)
    {
        bot.connectionRegistration();
        bot.join();
        bot.loop();
    }
}
