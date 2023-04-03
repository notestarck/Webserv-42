/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 12:22:31 by estarck           #+#    #+#             */
/*   Updated: 2023/04/03 11:33:55 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/utils.hpp"

std::vector<Server *>		_server;
std::vector<ParsConfig *>	_config;
Connection					_connection;
volatile sig_atomic_t						boolStart = 1;

int main(int argc, char ** argv)
{
	signal(SIGINT, signal_handler);
	//On verifie la presence du fichier .conf
	if (argc < 2)
	{
		std::cerr << "\033[1;31mError : Need < nginx_config_file >\033[0m" << std::endl;
		exit (-1);
	}
	
	//On verifie l'extension du fichier.
	std::string	extension(argv[1]);
	if (!checkExtension(extension))
	{
		std::cerr << "\033[1;31mError : Need extension .conf\033[0m" << std::endl;
		exit (-1);
	}

	//Ouverture du fichier
	const std::string  config_file_path(argv[1]);
	std::ifstream config_file(config_file_path.c_str());
	if (!config_file.is_open())
		std::cerr << "\033[1;31mError : Opening " << config_file_path << "\003[0m" << std::endl;

	unsigned int _nbrServer = countServer(config_file);

	//On compte le nombre de serveur qu'on enregistre ensuite dans _config.
	for (unsigned int i = 0; i < _nbrServer; i++)
	{
		ParsConfig *tmp = new ParsConfig(config_file);
		_config.push_back(tmp);
	}

	//Initialisation des Server.
	for (unsigned int i = 0; i < _nbrServer; i++)
	{
		Server *tmp = new Server(*_config[i]);
		_server.push_back(tmp);
	}

	//Crate connection
	Connection	_tmp(_server);
	_connection = _tmp;
	while (boolStart)
	{
		_connection.initConnection();
		_connection.runSelect();
		_connection.acceptSocket();
		_connection.traitement();
	}
	delProg();
	std::cout << "Fin du Programme\n";

    return 0;
}
