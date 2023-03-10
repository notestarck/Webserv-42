/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/10 16:08:03 by estarck           #+#    #+#             */
/*   Updated: 2023/03/10 16:41:24 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(ParsConfig &server) :
	_server(server)
{
	creatSocket();
	paramSocket();
}

Server::Server(const Server &srcs)
{ *this = srcs; }

Server::~Server()
{
	close(_idSocket);
}

Server & Server::operator=(const Server &srcs)
{}

void Server::creatSocket()
{
	_idSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(_idSocket == INVALID_SOCKET)
		cerr << "\033[1;31mError : socket\033[0m" << endl;
	cout << "\033[34mSocket created : \033[0m" << _idSocket << " en mode TCP/IP." << endl;
}

void Server::paramSocket()
{
	int	tmp;
	
	tmp = setsockopt(_idSocket, IPPROTO_TCP, TCP_NODELAY,(char *)&tmp, sizeof(tmp));
	if (tmp != 0)
		cerr << "\033[1;31mError : \033[0mparamSocket" << endl;
}

void Server::linkSocket()
{
	_infoSrcs.sin_family = AF_INET;
	_infoSrcs.sin_addr.s_addr = htonl(_server.getIp()); //need conversion
}