#include <SFML/Network.hpp>

#include "Bot.hpp"

int main()
{
    Bot bot;

    if(bot.connServer()==sf::Socket::Done)
    {
        bot.connReg();
        bot.join();
        bot.loop();
    }
}
