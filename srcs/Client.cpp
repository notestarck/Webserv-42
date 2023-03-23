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

Client::Client(const ParsConfig *config) :
	_config(config)
{}

Client::Client(const Client &srcs) :
    _config(srcs._config)
{ *this = srcs; }

Client::~Client()
{}

Client &Client::operator=(const Client &srcs)
{
	if (this != &srcs)
	{
		std::cout << "Constructeur d'operator== de client\n";
		_csock = srcs._csock;
		_csin = srcs._csin;
		_crecsize = srcs._crecsize;
		_lastGetTime = srcs._lastGetTime;
		_recSize = srcs._recSize;
		_config = srcs._config;
		_location = srcs._location;
	}
	return (*this);
}