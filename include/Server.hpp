/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/10 15:58:28 by estarck           #+#    #+#             */
/*   Updated: 2023/03/14 13:01:09 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef _SERVER_HPP_
#define _SERVER_HPP_

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
#include "ParsConfig.hpp"

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

typedef int SOCKET;


class Server
{
	public :
		Server(ParsConfig &server);
		Server(const Server &srcs);
		~Server();

		Server & operator=(const Server &srcs);

		SOCKET			getSocket() const;
		unsigned int	getPort() const;

		bool hasCapacity() const;
		void incrementCurrentConnection();
		void decrementCurrentConnection();

	private :
	//	Server();
		int			_sockError;
		ParsConfig	_server;
		int			_currentConnection;
		int			_maxConnection;

    	/* Socket et contexte d'adressage du serveur */
		SOCKET		_sock;
		sockaddr_in	_sin;
		socklen_t	_recsize;

		void		creatSocket();
		void		paramSocket();
		void		linkSocket();
		in_addr_t	convertIp(const std::string &str);
		void		listenTCP();

};//class Server

#endif /* _SERVER_HPP_ */