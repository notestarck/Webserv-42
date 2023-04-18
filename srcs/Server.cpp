/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/10 16:08:03 by estarck           #+#    #+#             */
/*   Updated: 2023/04/17 15:13:41 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(ParsConfig *server) :
	_sockError(0),
	_config(server),
	_currentConnection(0),
	_maxConnection(100),
	_recsize(sizeof(_sin)),
	_location(_config->getLocation()),
	_nbrLocation(_config->getNbrLocation())
{
	creatSocket();
	paramSocket();
	linkSocket();
	listenTCP();
}

Server::Server(const Server &srcs) :
	_config(srcs._config)
{ *this = srcs; }

Server::~Server()
{
	std::cout << "\033[32mLiberation des sockets serveur : " << getNameServer() <<  "\033[0m" <<std::endl; 
	closeSocket();
}

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
	return (*this);
}

//Init socket
void Server::creatSocket()
{
	//_sock = socket(AF_INET, SOCK_STREAM, 0);
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(_sock == INVALID_SOCKET)
	{
		std::cerr << "\033[1;31mError : Server::creatSocket socket() " << strerror(errno) << "\033[0m" << std::endl;
		exit (1);
	}
	if (fcntl(_sock, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "\033[1;31mError : Server::creatSocket fcntl() " << strerror(errno) << "\033[0m" << std::endl;
		exit (1);
	}
	std::cout << "\033[34mSocket created : \033[0m" << _sock << " en mode TCP/IP." << std::endl;
}

//Close socket
void Server::closeSocket()
{
    try 
	{
        if (_sock != INVALID_SOCKET) 
		{
            close(_sock);
            _sock = INVALID_SOCKET;
        }
    }
    catch (const std::exception& e) 
	{
        std::cerr << "Erreur lors de la fermeture de la socket : " << e.what() << std::endl;
    }
}

//Setsockopt
void Server::paramSocket()
{
	//a reverifier
	int	optval = 1;
	int tmp = setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); //Eviter d'avoir les erreurs du bind(), voir si cela pose d'autres soucis.
	if (tmp != 0)
		std::cerr << "\033[1;31mError : Server::paramSocket() \033[0mparamSocket" << std::endl;
}

//Bind socket
void Server::linkSocket()
{
	std::memset(&_sin, 0, sizeof(_sin));
	_sin.sin_port = htons(getPort());
	_sin.sin_addr.s_addr = convertIp(getHost()); //Pour une ecoute sur toutes les adresses htonl(INADDR_ANY)
	_sin.sin_family = AF_INET;
	_sockError = bind(_sock, (sockaddr*)&_sin, _recsize);
	if (_sockError != 0)
	{
		std::cerr << "\033[1;31mError : Server::linkSocket bind() " << strerror(errno) << "\033[0m" << std::endl;
		exit(1);
	}
}

//Config la socket du serveur pr une connection TCP
void	Server::listenTCP()
{
	_sockError = listen(_sock, _maxConnection);
	if (_sockError == SOCKET_ERROR)
	{
		std::cerr << "\033[1;31mError : Server::linstenTCP listen() " << strerror(errno) << "\033[0m" << std::endl;
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

Server	&Server::getServer()
{ return (*this); }

std::string		Server::getHost() const
{ return (_config->getHost()); }
		
unsigned int Server::getPort() const
{ return (_config->getPort()); }

std::string		Server::getNameServer() const
{ return (_config->getNameServer()); }

std::string		Server::getRoot() const
{ return (_config->getRoot()); }

std::string		Server::getIndex() const
{ return (_config->getIndex()); }

std::string		Server::getErrorPage(int code) const
{ return (_config->getErrorPage(code)); }

size_t			Server::getNbrLocation() const
{ return (_config->getNbrLocation()); }

ParsConfig &Server::getConfig()
{ return (*_config); }

std::vector<ParsConfig::Location> &Server::getLocation()
{ return (_location); }

int	Server::getCurrentConnection()
{ return (_currentConnection); }

/// @brief Retourne vrai si y'a de la place sur le serveur.
/// @return bool
bool	Server::hasCapacity() const
{ return (_currentConnection < _maxConnection); }

void	Server::incrementCurrentConnection()
{ ++_currentConnection; }

void	Server::decrementCurrentConnection()
{ --_currentConnection; }