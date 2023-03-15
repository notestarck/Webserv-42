/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 13:50:45 by estarck           #+#    #+#             */
/*   Updated: 2023/03/15 15:53:58 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Connection.hpp"

Connection::Connection()
{}

Connection::Connection(std::vector<Server *> &servers) :
	_servers(servers),
	_maxFd(-1)
{
	FD_ZERO(&_read);
	FD_ZERO(&_write);
	
	_timeout.tv_sec = 1;
	_timeout.tv_usec = 0;

	while(42)
		initConnection();
}

Connection::Connection(const Connection & srcs)
{}

Connection::~Connection()
{}

Connection & Connection::operator=(const Connection &srcs)
{
	return (*this);
}

void Connection::initConnection()
{
	for (std::vector<Server *>::iterator it = _servers.begin(); it < _servers.end(); it++)
		initSelect((*it)->getSocket(), _read);
	for (std::vector<Client>::iterator it = _client.begin(); it < _client.end(); it++)
		initSelect(it->_csock, _read);
	runSelect();
	acceptSocket();
}

void Connection::initSelect(int fd, fd_set &set)
{
	FD_SET(fd, &set);
	if (fd > _maxFd)
		_maxFd = fd;
}

void Connection::runSelect()
{
	int res = select(_maxFd + 1, &_read, &_write, &_errors, 0);
	if (res < 0)
	{
		std::cerr << "Error : Status of socket Connection::runSelect()" << std::endl;
		exit (1);
	}
	else if (res == 0)
		std::cerr << "Error : Timeout Select Connection::runSelect()" << std::endl;
}

void Connection::acceptSocket()
{
	for (std::vector<Server *>::iterator it = _servers.begin(); it < _servers.end(); it++)
	{
		if (FD_ISSET((*it)->getSocket(), &_read))
		{
			//Socket server est pret a etre lu
			Client	newClient;
			if((*it)->hasCapacity())
			{
				int client_fd = accept((*it)->getSocket(), (sockaddr *)&newClient._csin, &newClient._crecsize);
				if (client_fd < 0)
					std::cerr << "Failed to accept connection on port " << (*it)->getPort() << std::endl;
				else
				{
					(*it)->incrementCurrentConnection();
					_client.push_back(newClient);
					std::cout << "Accepted connection on port " << (*it)->getPort() << std::endl;
				}
			}
			else
			{
				std::cerr << "Connection limit reached on port " << (*it)->getPort() << std::endl;
				//boucle infini a gerer.
			}
		}
	}
}