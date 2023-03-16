/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
<<<<<<< HEAD
/*   Created: 2023/03/14 10:50:28 by estarck           #+#    #+#             */
/*   Updated: 2023/03/14 15:29:29 by estarck          ###   ########.fr       */
=======
/*   Created: 2023/03/15 15:36:30 by estarck           #+#    #+#             */
/*   Updated: 2023/03/16 14:33:44 by estarck          ###   ########.fr       */
>>>>>>> master
/*                                                                            */
/* ************************************************************************** */

#ifndef _MASTER_HPP_
#define _MASTER_HPP_

//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netinet/tcp.h>
//#include <arpa/inet.h>
//#include <unistd.h>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <vector>
//#include <map>
#include <string>

#include "./ParsConfig.hpp"
#include "./Server.hpp"
#include "./Connection.hpp"

#include <vector>
#include <map>

class Master
{
	public :
		Master(std::ifstream &config_file, unsigned int nbrServer);
		Master(const Master &srcs);
		~Master();

		Master &operator=(const Master &srcs);

	private :
		// @brief fichier de config parser et ranger dans un vector, 1 element par server
		std::vector<ParsConfig *>	_config;

		// @brief Initialise chaque serveur a partir de _config
		std::vector<Server *>		_server;
		unsigned int				_nbrServer;

		// @brief gestion des connexion client avec le serveur
		Connection					_connection;

		//Methode private
		void initConfig(std::ifstream &config_file);
		void initServer();
		void creatConnection();
		void startConnection();
		
};//class Master

#endif/* _MASTER_HPP_ */
