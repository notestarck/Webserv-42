/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/23 11:08:07 by estarck           #+#    #+#             */
/*   Updated: 2023/04/11 11:27:50 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client(ParsConfig &config, Server &server, std::vector<ParsConfig::Location> &location) :
	_keepAlive(true),
	_httpVersion(""),
	_uri(""),
	_method(UNKNOWN),
	_body(""),
	_config(config),
	_server(server),
	_location(location)

{
	_csock = 0;
}

Client::Client(const Client &srcs) :
    _config(srcs._config),
	_server(srcs._server),
	_location(srcs._location)
{ *this = srcs; }

Client::~Client()
{}

Client &Client::operator=(const Client &srcs)
{
	if (this != &srcs)
	{
		_csock = srcs._csock;
		_csin = srcs._csin;
		_crecSize = srcs._crecSize;
		_lastGetTime = srcs._lastGetTime;
		_keepAlive = srcs._keepAlive;
		_requestStr = srcs._requestStr;
		_method = srcs._method;
		_uri = srcs._uri;
		_httpVersion = srcs._httpVersion;
		_headers = srcs._headers;
		_body = srcs._body;
		_config = srcs._config;
		_location = srcs._location;
		_server = srcs._server;
	}
	return (*this);
}