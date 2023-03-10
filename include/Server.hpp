/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/10 15:58:28 by estarck           #+#    #+#             */
/*   Updated: 2023/03/10 16:38:11 by estarck          ###   ########.fr       */
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
#include <./ParsConfig/ParsConfig.hpp>

#define INVALID_SOCKET -1

typedef int SOCKET;

using namespace std;

class Server
{
	public :
		Server(ParsConfig &server);
		Server(const Server &srcs);
		~Server();

		Server & operator=(const Server &srcs);


	private :
		ParsConfig	_server;
		SOCKET		_idSocket;
		sockaddr_in	_infoSrcs;

		void creatSocket();
		void paramSocket();
		void linkSocket();
		
	
	
};// class Server

#endif /* _SERVER_HPP_ */