/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/10 16:08:03 by estarck           #+#    #+#             */
/*   Updated: 2023/03/16 14:37:55 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(ParsConfig &server) :
	_config(server),
	_location(_config.getLocation()),
	_nbrLocation(_config.getNbrLocation()),
	_sockError(0),
	_maxConnection(10),
	_recsize(sizeof(_sin))
{
	creatSocket();
	paramSocket();
	linkSocket();
	listenTCP();
}

Server::Server(const Server &srcs)
{ *this = srcs; }

Server::~Server()
{ close(_sock); }

Server & Server::operator=(const Server &srcs)
{ 
	if (this != &srcs)
	{
		_sockError = srcs._sockError;
		_config = srcs._config;
		_currentConnection = srcs._currentConnection;
		_maxConnection = srcs._maxConnection;
		_sock = srcs._sock;
		_sin = srcs._sin;
		_recsize = srcs._recsize;
		_location = srcs._location;
		_nbrLocation = srcs._nbrLocation;
	}
return (*this); }

void Server::creatSocket()
{
	_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(_sock == INVALID_SOCKET)
	{
		std::cerr << "\033[1;31mError : socket\033[0m" << std::endl;
		exit (1);
	}
	std::cout << "\033[34mSocket created : \033[0m" << _sock << " en mode TCP/IP." << std::endl;
}

void Server::paramSocket()
{
	int	tmp;
	
	tmp = setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY,(char *)&tmp, sizeof(tmp));
	if (tmp != 0)
		std::cerr << "\033[1;31mError : Server::paramSocket() \033[0mparamSocket" << std::endl;
}

void Server::linkSocket()
{
	_sin.sin_port = htons(getPort());
	_sin.sin_addr.s_addr = convertIp(getHost()); //Pour une estd::coute sur std::coutes les adresses htonl(INADDR_ANY)
	_sin.sin_family = AF_INET;
	_sockError = bind(_sock, (sockaddr*)&_sin, _recsize);
	if (_sockError == SOCKET_ERROR)
	{
		std::cerr << "\033[1;31mError : Server::linkSocket bind()\033[0m" << std::endl;
		exit(1);
	}
}

void	Server::listenTCP()
{
	_sockError = listen(_sock, _maxConnection);
	if (_sockError == SOCKET_ERROR)
	{
		std::cerr << "\033[31mError : Server::listenTCP()\033[0m" << std::endl;
		exit (1);
	}
	std::cout << "\033[33mlistenTCP() - Ecoute du port " << getPort() << "\033[0m" << std::endl;
}

in_addr_t Server::convertIp(const std::string &str)
{
	std::vector<std::string>	octets;
    std::stringstream			ss(str);
    std::string					token;

    while (getline(ss, token, '.'))
        octets.push_back(token);
    if (octets.size() != 4)
	{
        std::cerr << "\033[33mError: invalid IP address Server::convertIp() " << str << "\033[0m" << std::endl;
        exit(1);
    }
    in_addr_t addr = 0;
    for (int i = 3; i >= 0; --i)
	{
        try
		{
            int octet = stoi(octets[i]);
            if (octet < 0 || octet > 255) {
                std::cerr << "Error: invalid IP address " << str << std::endl;
                exit(-1);
            }
            addr |= static_cast<in_addr_t>(octet) << ((3 - i) * 8);
        }
		catch (const std::exception& e)
		{
            std::cerr << "\033[33mError: invalid IP address Server::convertIp() " << str << "\033[0m" << std::endl;
            exit(1);
        }
    }
    return htonl(addr);
}

SOCKET	Server::getSocket() const
{ return (_sock); }

std::string		Server::getHost() const
{ return (_config.getHost()); }
		
unsigned int Server::getPort() const
{ return (_config.getPort()); }

std::string		Server::getNameServer() const
{ return (_config.getNameServer()); }

std::string		Server::getRoot() const
{ return (_config.getRoot()); }

std::string		Server::getIndex() const
{ return (_config.getIndex()); }

std::string		Server::getErrorPage(int code) const
{ return (_config.getErrorPage(code)); }

size_t			Server::getNbrLocation() const
{ return (_config.getNbrLocation()); }

/// @brief Retourne vrai si y'a de la place sur le serveur.
/// @return bool
bool	Server::hasCapacity() const
{ return (_currentConnection < _maxConnection); }

void	Server::incrementCurrentConnection()
{ ++_currentConnection; }

void	Server::decrementCurrentConnection()
{ --_currentConnection; }