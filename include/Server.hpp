/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/10 15:58:28 by estarck           #+#    #+#             */
/*   Updated: 2023/03/16 14:59:06 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <iostream>

#include "./ParsConfig.hpp"

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

<<<<<<< HEAD
		//Getteur
		SOCKET			getSocket() const;
		std::string		getHost() const;
		SOCKET			getSocket() const;
		unsigned int	getPort() const;
>>>>>>> 427a1fd6ceeddf9831520b4a3025cdded7f9f907
		void decrementCurrentConnection();
		ParsConfig							_config;
		SOCKET								_sock;
		std::vector<ParsConfig::Location>	_location;
		void		paramSocket();
};//class Server
