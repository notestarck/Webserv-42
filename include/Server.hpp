/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/10 15:58:28 by estarck           #+#    #+#             */
/*   Updated: 2023/03/13 09:50:11 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "ParsConfig.hpp"

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

typedef int SOCKET;

using namespace std;

class Server
{
	public :
		Server(ParsConfig &server);
		Server(const Server &srcs);
		~Server();

		Server & operator=(const Server &srcs);

		void		acceptConnect();

	private :
	//	Server();
		int			_sockError;
		ParsConfig	_server;
    	/* Socket et contexte d'adressage du serveur */
		SOCKET		_sock;
		sockaddr_in	_sin;
		socklen_t	_recsize;

    	/* Socket et contexte d'adressage du client */
		SOCKET		_csock;
		sockaddr_in	_csin;
		socklen_t	_crecsize;

		void		creatSocket();
		void		paramSocket();
		void		linkSocket();
		in_addr_t	convertIp(const string &str);
		void		listenTCP();

		
	
	
};// class Server

#endif /* _SERVER_HPP_ */