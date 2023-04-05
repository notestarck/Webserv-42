/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:32:36 by estarck           #+#    #+#             */
/*   Updated: 2023/03/23 16:14:04 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include <netinet/tcp.h>
#include <sys/socket.h>

#include "./Server.hpp"
#include "./ParsConfig.hpp"

#define MAX_REQUEST_SIZE 2048

struct Client
{
	Client(ParsConfig &config, Server &server, std::vector<ParsConfig::Location> &location);
	Client (const Client &srcs);

	~Client();
	Client &operator=(const Client &srcs);
    /* Socket et contexte d'adressage du client */
	int					_csock;
	sockaddr_storage	_csin;
	socklen_t			_crecsize;
	timeval				_lastGetTime;

	char				_recBuffer[MAX_REQUEST_SIZE + 1];
	int					_recSize;
    //Config du serveur
    ParsConfig									&_config;
    std::vector<ParsConfig::Location>			&_location;
	Server										&_server;


};//struct Client

#endif/* _CLIENT_HPP_ */