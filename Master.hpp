/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 15:36:30 by estarck           #+#    #+#             */
/*   Updated: 2023/03/15 15:49:51 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _MASTER_HPP_
#define _MASTER_HPP_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "Client.hpp"
#include "Connection.hpp"
#include "ParsConfig.hpp"
#include "Server.hpp"


class Master
{
	public :
		Master(std::ifstream &config_file);
		Master(const Master &srcs);
		~Master();

		Master &operator=(const Master &srcs);

	private :
		// @brief fichier de config parser et ranger dans un vector, 1 element par server
		std::vector<ParsConfig *>	_config;

		// @brief Initialise chaque serveur a partir de _config
		std::vector<Server *>		_server;
		int							_nbrServer;

		// @brief gestion des connexion client avec le serveur
		Connection					_connection;
		
};//class Master

#endif/* _MASTER_HPP_ */