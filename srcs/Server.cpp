/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/10 16:08:03 by estarck           #+#    #+#             */
/*   Updated: 2023/03/13 14:07:20 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(ParsConfig &server) :
	_server(server),
	_sockError(0),
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
{
	//a voir s'il faut le mettre ici.
	//close(_idSocket);
}

Server & Server::operator=(const Server &srcs)
{ return (*this); }

void Server::creatSocket()
{
	_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(_sock == INVALID_SOCKET)
		std::cerr << "\033[1;31mError : socket\033[0m" << std::endl;
	std::cout << "\033[34mSocket created : \033[0m" << _sock << " en mode TCP/IP." << std::endl;
}

void Server::paramSocket()
{
	int	tmp;
	
	tmp = setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY,(char *)&tmp, sizeof(tmp));
	if (tmp != 0)
		std::cerr << "\033[1;31mError : \033[0mparamSocket" << std::endl;
}

void Server::linkSocket()
{
	_sin.sin_port = htons(_server.getPort());
	_sin.sin_addr.s_addr = convertIp(_server.getIp()); //Pour une estd::coute sur std::coutes les adresses htonl(INADDR_ANY)
	_sin.sin_family = AF_INET;
	_sockError = bind(_sock, (sockaddr*)&_sin, _recsize);
	if (_sockError == -1)
	{
		std::cerr << "\033[1;31mError : linkSocket\033[0m" << std::endl;
		exit(-5);
	}
}

in_addr_t Server::convertIp(const std::string &str)
{
	std::vector<std::string>	octets;
    std::stringstream	ss(str);
    std::string			token;

    while (getline(ss, token, '.'))
        octets.push_back(token);
    if (octets.size() != 4)
	{
        std::cerr << "Error: invalid IP address " << str << std::endl;
        exit(-1);
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
            std::cerr << "Error: invalid IP address " << str << std::endl;
            exit(-1);
        }
    }
    return htonl(addr);
}

void	Server::listenTCP()
{
	_sockError = listen(_sock, 10);
	if (_sockError == -1)
		std::cerr << "\033[31mError : listenTCP()\033[0m" << std::endl;
	std::cout << "\033[33mlistenTCP() - Ecoute du port " << _server.getPort() << "\033[0m" << std::endl;
}

//void	Server::acceptConnect()
//{
//	_csock = accept(_sock, (sockaddr*)&_csin, &_crecsize);
//	std::cout << "Un client se connecte sur le port : " << _server.getPort() << " - avec la socket " << _csock << " de " << inet_ntoa(_csin.sin_addr) << ":" << htons(_csin.sin_port) << std::endl;
//}

SOCKET	Server::getSocket()
{
	return (_sock);
}