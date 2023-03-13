/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/10 16:08:03 by estarck           #+#    #+#             */
/*   Updated: 2023/03/13 11:06:56 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(ParsConfig &server) :
	_server(server),
	_sockError(0)
{
	memset(&_sin, 0, sizeof(_sin));
    memset(&_recsize, 0, sizeof(_recsize));
	memset(&_csin, 0, sizeof(_csin));
    memset(&_crecsize, 0, sizeof(_crecsize));
	creatSocket();
	paramSocket();
	linkSocket();
	listenTCP();
}

Server::Server(const Server &srcs)
{ *this = srcs; }

Server::~Server()
{
	cout << "attention" << endl;
	//a voir s'il faut le mettre ici.
	//close(_idSocket);
}

Server & Server::operator=(const Server &srcs)
{ return (*this); }

void Server::creatSocket()
{
	_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(_sock == INVALID_SOCKET)
		cerr << "\033[1;31mError : socket\033[0m" << endl;
	cout << "\033[34mSocket created : \033[0m" << _sock << " en mode TCP/IP." << endl;
}

void Server::paramSocket()
{
	int	tmp;
	
	tmp = setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY,(char *)&tmp, sizeof(tmp));
	if (tmp != 0)
		cerr << "\033[1;31mError : \033[0mparamSocket" << endl;
}

void Server::linkSocket()
{
	_sin.sin_port = htons(_server.getPort());
	_sin.sin_addr.s_addr = convertIp(_server.getIp()); //Pour une ecoute sur toutes les adresses htonl(INADDR_ANY)
	_sin.sin_family = AF_INET;
	_recsize = sizeof(_sin);
	_sockError = bind(_sock, (sockaddr*)&_sin, _recsize);
	if (_sockError != 0)
	{
		cerr << "\033[1;31mError : linkSocket\033[0m" << endl;
		exit(-5);
	}
}

in_addr_t Server::convertIp(const string &str)
{
	vector<string>	octets;
    stringstream	ss(str);
    string			token;

    while (getline(ss, token, '.'))
        octets.push_back(token);
    if (octets.size() != 4)
	{
        cerr << "Error: invalid IP address " << str << endl;
        exit(-1);
    }
    in_addr_t addr = 0;
    for (int i = 3; i >= 0; --i)
	{
        try
		{
            int octet = stoi(octets[i]);
            if (octet < 0 || octet > 255) {
                cerr << "Error: invalid IP address " << str << endl;
                exit(-1);
            }
            addr |= static_cast<in_addr_t>(octet) << ((3 - i) * 8);
        }
		catch (const exception& e)
		{
            cerr << "Error: invalid IP address " << str << endl;
            exit(-1);
        }
    }
    return htonl(addr);
}

void	Server::listenTCP()
{
	_sockError = listen(_sock, 10);
	if (_sockError == -1)
		cerr << "\033[31mError : listenTCP()\033[0m" << endl;
	cout << "\033[33mlistenTCP() - Ecoute du port " << _server.getPort() << "\033[0m" << endl;
}

void	Server::acceptConnect()
{
	cout << "Patientez pendant que le client se connecte sur le port " << _server.getPort() << "...\n"; 
	_csock = accept(_sock, (sockaddr*)&_csin, &_crecsize);
	cout << _csin.sin_port << endl;
	cout << "Un client se connecte avec la socket " << _csock << " de " << inet_ntoa(_csin.sin_addr) << ":" << htons(_csin.sin_port) << endl;
}