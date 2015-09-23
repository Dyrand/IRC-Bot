#include <iostream>

#include <SFML/network.hpp>
#include "TcpSocket.hpp"

sf::Socket::Status TcpSocket::connect(const sf::IpAddress &remoteAddress, unsigned short remotePort, sf::Time timeout = sf::Time::Zero)
{
	currentStatus = sf::TcpSocket::connect(remoteAddress,remotePort,timeout);
	currentStatusType = StatusType::Connect;
	server = remoteAddress;

	if(debugMode)
	{outputStatus();}

	return currentStatus;
}

sf::Socket::Status TcpSocket::send(const void *data, std::size_t size)
{
	currentStatus = sf::TcpSocket::send(data,size);
	currentStatusType = StatusType::Send;

	if(debugMode)
	{outputStatus();}

	return currentStatus;
}

sf::Socket::Status TcpSocket::send(const void *data, std::size_t size, std::size_t &sent)
{
	currentStatus = sf::TcpSocket::send(data,size,sent);
	currentStatusType = StatusType::Send;

	if(debugMode)
	{outputStatus();}

	return currentStatus;
}

sf::Socket::Status TcpSocket::receive(void *data, std::size_t size, std::size_t &received)
{
	currentStatus = sf::TcpSocket::receive(data,size,received);
	currentStatusType = StatusType::Receive;

	if(debugMode)
	{outputStatus();}

	return currentStatus;
}

sf::Socket::Status TcpSocket::send(sf::Packet &packet)
{
	currentStatus = sf::TcpSocket::send(packet);
	currentStatusType = StatusType::Send;

	if(debugMode)
	{outputStatus();}

	return currentStatus;
}

sf::Socket::Status TcpSocket::receive(sf::Packet &packet)
{
	currentStatus = sf::TcpSocket::receive(packet);
	currentStatusType = StatusType::Receive;

	if(debugMode)
	{outputStatus();}

	return currentStatus;
}

sf::Socket::Status TcpSocket::getCurrentStatus()
{
	return currentStatus;
}

bool TcpSocket::setDebugMode(bool mode)
{
	if(mode == debugMode)
	{
		return false;
	}

	debugMode = mode;
	return true;
}

void TcpSocket::outputStatus()
{
	if(currentStatusType == StatusType::Connect)
	{
		std::cout << "(Connect) ";
		switch(currentStatus)
        {
        case sf::Socket::Done:
            std::cout << "Connection to " << server.toString() << " done.\n";
        break;
    
        case sf::Socket::NotReady:
            std::cout << "Connection to " << server.toString() << " not ready.\n";
        break;
    	
   		case sf::Socket::Partial:
   			std::cout << "The TCP socket sent a part of the data.\n";
   		break;

        case sf::Socket::Disconnected:
            std::cout << "The TCP socket has been disconnected.\n";
        break;
    
        case sf::Socket::Error:
        default:
            std::cout << "Error connecting to " << server.toString() << ".\n";
        break;
        }
	}
	else if(currentStatusType == StatusType::Send)
	{
		std::cout << "(Send) ";
		switch(currentStatus)
        {
        case sf::Socket::Done:
            std::cout << "The socket has sent the data.\n";
        break;
    
        case sf::Socket::NotReady:
            std::cout << "The socket is not ready to send the data yet.\n";
        break;
    	
   		case sf::Socket::Partial:
   			std::cout << "The TCP socket sent a part of the data.\n";
   		break;

        case sf::Socket::Disconnected:
            std::cout << "The TCP socket has been disconnected.\n";
        break;
    
        case sf::Socket::Error:
        default:
            std::cout << "An unexpected error happened.\n";
        break;
        }
	}
	else if(currentStatusType == StatusType::Receive)
	{
		std::cout << "(Receive) ";
		switch(currentStatus)
        {
        case sf::Socket::Done:
            std::cout << "The socket has received the data.\n";
        break;
    
        case sf::Socket::NotReady:
            std::cout << "The socket is not ready to receive the data yet.\n";
        break;
    	
   		case sf::Socket::Partial:
   			std::cout << "The TCP socket received a part of the data.\n";
   		break;

        case sf::Socket::Disconnected:
            std::cout << "The TCP socket has been disconnected.\n";
        break;
    
        case sf::Socket::Error:
        default:
            std::cout << "An unexpected error happened.\n";
        break;
        }
	}
}