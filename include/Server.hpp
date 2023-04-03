/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/10 15:58:28 by estarck           #+#    #+#             */
/*   Updated: 2023/03/17 14:26:51 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
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

		//Getteur
		SOCKET									getSocket() const;
		Server									&getServer();
		std::string								getHost() const;
		unsigned int							getPort() const;
		std::string								getNameServer() const;
		std::string								getRoot() const;
		std::string								getIndex() const;
		std::string								getErrorPage(int code) const;
		size_t									getNbrLocation() const;
		const ParsConfig						&getConfig() const;
		const std::vector<ParsConfig::Location>	&getLocation() const;

		//nbr Connection
		bool hasCapacity() const;
		void incrementCurrentConnection();
		void decrementCurrentConnection();

	private :
	//	Server();
		int									_sockError;
		ParsConfig							_config;
		int									_currentConnection;
		int									_maxConnection;

    	/* Socket et contexte d'adressage du serveur */
		SOCKET								_sock;
		sockaddr_in							_sin;
		socklen_t							_recsize;
		
		/* Access aux Location du fichier de config */
		std::vector<ParsConfig::Location>	_location;
		size_t								_nbrLocation;

		/* Private Methode */
		void		creatSocket();
		void		paramSocket();
		void		linkSocket();
		void		listenTCP();
		in_addr_t	convertIp(const std::string &str);

};//class Server

#endif /* _SERVER_HPP_ */