/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 15:50:10 by estarck           #+#    #+#             */
/*   Updated: 2023/03/16 09:18:28 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Master.hpp"

Master::Master(std::ifstream &config_file, unsigned int nbrServer) :
	_nbrServer(nbrServer)
{
	initConfig(config_file);
}

Master::Master(const Master &srcs)
{}

Master::~Master()
{}

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
	for (unsigned int i = 0; )
}