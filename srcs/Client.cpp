/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/23 11:08:07 by estarck           #+#    #+#             */
/*   Updated: 2023/03/23 16:14:15 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client(const ParsConfig *config, Server &server) :
	_config(config),
	_server(server)

{
	_csock = 0;
	_recSize = 0;
}

Client::Client(const Client &srcs) :
    _config(srcs._config),
	_server(srcs._server)
{ *this = srcs; }

Client::~Client()
{}

Client &Client::operator=(const Client &srcs)
{
	if (this != &srcs)
	{
		_csock = srcs._csock;
		_csin = srcs._csin;
		_crecsize = srcs._crecsize;
		_lastGetTime = srcs._lastGetTime;
		_recSize = srcs._recSize;
		_config = srcs._config;
		_location = srcs._location;
		_server = srcs._server;
		for (size_t i = 0; i < srcs._location.size(); i++)
		{
			for (std::vector<std::string>::const_iterator it = srcs._location[i]._allow.begin(); it < srcs._location[i]._allow.end(); it++)
				_location[i]._allow.push_back(*it);
		}
	}
	return (*this);
}