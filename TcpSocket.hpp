#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP

#include <SFML/Network.hpp>

class TcpSocket: public sf::TcpSocket
{
	public:
		enum class StatusType 
		{
			Connect, 
			Send, 
			Receive
		};

		TcpSocket(bool debugMode_t = true, sf::Socket::Status currentStatus_t = sf::Socket::NotReady, StatusType currentStatusType_t = StatusType::Connect):
			debugMode(debugMode_t),
			currentStatus(currentStatus_t),
			currentStatusType(currentStatusType_t)
		{}
		sf::Socket::Status connect(const sf::IpAddress &remoteAddress, unsigned short remotePort, sf::Time timeout);
		sf::Socket::Status send(const void *data, std::size_t size);
		sf::Socket::Status send(const void *data, std::size_t size, std::size_t &sent);
		sf::Socket::Status receive(void *data, std::size_t size, std::size_t &received);
		sf::Socket::Status send(sf::Packet &packet);
		sf::Socket::Status receive(sf::Packet &packet);

		sf::Socket::Status getCurrentStatus();
		bool setDebugMode(bool mode); //returns true if the mode was changed, false otherwise
		void outputStatus();
	protected:
	private:
		bool debugMode;
		sf::Socket::Status currentStatus;
		StatusType currentStatusType;
		sf::IpAddress server;
};

#endif // TCP_SOCKET_HPP