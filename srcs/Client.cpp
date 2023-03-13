/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/13 14:24:11 by estarck           #+#    #+#             */
/*   Updated: 2023/03/13 15:11:26 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client(unsigned int nbrServer, std::vector<ParsConfig *> &config, std::vector<Server *> &server) :
	_crecsize(sizeof(_csin)),
	_maxFd(-1),
	_nbrServer(nbrServer),
	_config(config),
	_server(server)
{
	acceptConnection();
}

Client::Client(const Client &srcs)
{}

Client::~Client()
{}

Client & Client::operator=(const Client &srcs)
{ return (*this); }

void Client::acceptConnection()
{
	FD_ZERO(&_readFds);
	for(int i = 0; i < _nbrServer; i++)
	{
		FD_SET(_server[i]->getSocket(), &_readFds);
		if(_server[i]->getSocket() > _maxFd)
			_maxFd = _server[i]->getSocket();
	}
	
	while(42)
	{
		fd_set	tmpFds = _readFds;
		if(select(_maxFd + 1, &tmpFds, nullptr, nullptr, nullptr) < 0)
		{
			std::cerr << "Error : acceptConnection() slect()" << std::endl;
			exit(-7);
		}
		for(int i = 0; i < _nbrServer; i++)
		{
			if(FD_ISSET(_server[i]->getSocket(), &tmpFds))
			{
				if (_server[i]->hasCapacity())
				{
					int client_fd = accept(_server[i]->getSocket(), (sockaddr *)&_csin, &_crecsize);
					if(client_fd < 0)
						std::cerr << "Failed to accept connection on port " << _config[i]->getPort() << std::endl;
					else
					{
						_server[i]->inrementCurrentConnection();
						std::cout << "Accepted connection on port " << _config[i]->getPort() << std::endl;
					}
				}
				else
					std::cerr << "Connection limit reached on port " << _config[i]->getPort() << std::endl;
			}
		}
		
	}
}