/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:32:36 by estarck           #+#    #+#             */
/*   Updated: 2023/04/10 21:19:16 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include <netinet/tcp.h>
#include <sys/socket.h>

#include "./Server.hpp"
#include "./ParsConfig.hpp"
#include "./HTTPRequest.hpp"

#define MAX_REQUEST_SIZE 1024

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

	//Request du client
	std::string							_requestStr;
	// Request HTTP parsee.
    std::string							_httpVersion;
    std::string							_uri;
    HTTPMethod							_method;
    /// @brief Uniform Resource Identifier, chaîne de caractères identifiant une ressource sur un réseau, la syntaxe respecte la norme World Wide Web.
    std::map<std::string, std::string>	_headers;
	std::string							_body;

    //Config du serveur
    ParsConfig									&_config;
	Server										&_server;
    std::vector<ParsConfig::Location>			&_location;

};//struct Client

#endif/* _CLIENT_HPP_ */