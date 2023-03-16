/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 15:50:10 by estarck           #+#    #+#             */
/*   Updated: 2023/03/16 14:36:53 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Master.hpp"

Master::Master(std::ifstream &config_file, unsigned int nbrServer) :
	_nbrServer(nbrServer)
{
	initConfig(config_file);
	initServer();
	creatConnection();
	startConnection();
}

Master::Master(const Master &srcs)
{}

Master::~Master()
{
	//delete _config
	for (unsigned int i = 0; i < _nbrServer; i++)
		delete _config[i];
		
	//delete _server
	for (unsigned int i = 0; i < _nbrServer; i++)
		delete _server[i];
}

Master &Master::operator=(const Master &srcs)
{ return (*this); }

void Master::initConfig(std::ifstream &config_file)
{
	for (unsigned int i = 0; i < _nbrServer; i++)
	{
		ParsConfig *tmp = new ParsConfig(config_file, i);
		_config.push_back(tmp);
	}
}

void Master::initServer()
{
	for (unsigned int i = 0; i < _nbrServer; i++)
	{
		Server *tmp = new Server(*_config[i]);
		_server.push_back(tmp);
	}
}

void Master::creatConnection()
{
	Connection tmp(_server);
	_connection = tmp;
}

void Master::startConnection()
{
	while (42)
	{
		_connection.initConnection();
		_connection.runSelect();
		_connection.acceptSocket();
		_connection.traitement();
	}
}