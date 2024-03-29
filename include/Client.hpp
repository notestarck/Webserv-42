/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:32:36 by estarck           #+#    #+#             */
/*   Updated: 2023/04/18 15:27:33 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include <netinet/tcp.h>
#include <sys/socket.h>

#include "./Server.hpp"
#include "./ParsConfig.hpp"
#include "./HTTPRequest.hpp"

enum HTTPMethod {
    GET,
    POST,
    DELETE,
    UNKNOWN
};

struct Client
{
	Client(ParsConfig &config, Server &server, std::vector<ParsConfig::Location> &location);
	Client (const Client &srcs);

	~Client();
	Client &operator=(const Client &srcs);


    /* Socket et contexte d'adressage du client */
	int					_csock;
	sockaddr_storage	_csin;
	socklen_t			_crecSize;
	timeval				_lastGetTime;

	/* Le client va t'il vivre ou mourir */
	bool				_keepAlive;

	// Request du client
	std::stringstream							_requestStr;
	ssize_t										_contentLenght;
	ssize_t										_sizeBody;
	bool										_requestPars;

	// Request HTTP parsee.
    std::string									_httpVersion;
    std::string									_uri;
    HTTPMethod									_method;
    std::map<std::string, std::string>			_headers;
	std::stringstream							_bodyReq;
	std::string                                 _cookie;
	std::string                                 _query;

	// Reponse du serveur
	std::string									_filePath;
	std::string									_bodyRep;
	std::string									_response;
	ssize_t										_sizeSend;
	ssize_t										_sizeRep;

	//Autoindex
	std::vector<std::pair<std::pair<std::string, std::string>, int> > _autoIndex;
	
    // Config du serveur
    ParsConfig									&_config;
	Server										&_server;
    std::vector<ParsConfig::Location>			&_location;
	
};//struct Client

#endif/* _CLIENT_HPP_ */