/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 15:36:30 by estarck           #+#    #+#             */
/*   Updated: 2023/03/16 15:03:14 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _MASTER_HPP_
#define _MASTER_HPP_

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