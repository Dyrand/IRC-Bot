#include <SFML/Network.hpp>

#include "Bot.hpp"

int main(int argc, char *argv[])
{
    Bot bot;

    if(bot.connectToServer()==sf::Socket::Done)
    {
        bot.registerConnection();
        bot.join();
        bot.loop();
    }
}
