/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/13 14:18:05 by estarck           #+#    #+#             */
/*   Updated: 2023/03/14 10:28:53 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "../include/ParsConfig.hpp"
#include "../include/Server.hpp"

class Client
{
	public :
		Client(unsigned int nbrServer, std::vector<ParsConfig *> &config, std::vector<Server *> &server);
		Client(const Client &srcs);
		~Client();

		Client &operator=(const Client &srcs);

	private :
    	/* Socket et contexte d'adressage du client */
		int			_csock;
		sockaddr_in	_csin;
		socklen_t	_crecsize;

		/* Gestion des fd */
		fd_set		_readFds;
		int			_maxFd;

		/* Serveur */
		int							_nbrServer;
		std::vector<ParsConfig *>	_config;
		std::vector<Server *>		_server;

		void	acceptConnection();
		
};//class Client

#endif/* _CLIENT_HPP_ */