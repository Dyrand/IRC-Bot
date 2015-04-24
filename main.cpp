#include <SFML/Network.hpp>

#include "Bot.hpp"

int main()
{
    Bot bot;

    if(bot.connectServer()==sf::Socket::Done)
    {
        bot.registerConnection();
        bot.join();
        bot.loop();
    }
}
