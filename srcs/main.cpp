/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 12:22:31 by estarck           #+#    #+#             */
/*   Updated: 2023/03/16 18:39:20 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <iostream>
#include <fstream>
#include <string>

#include "../include/ParsConfig.hpp"
#include "../include/Server.hpp"
#include "../include/Connection.hpp"

#include <vector>

void signal_handler(int signal)
{
    std::cout << "Signal SIGINT reçu. Fermeture du programme..." << std::endl;
    if (signal == SIGINT)
    	exit(1);
}

int countServer(std::ifstream &config_file)
{
	int     nbr_server = 0;
	char    *buf;

	while (!config_file.eof())
	{
		std::string  line;

		getline(config_file, line);
		if (line == "server {")
			nbr_server++;
	}
	config_file.seekg(0);
	return (nbr_server);
}

bool	checkExtension(std::string extension)
{
	std::string tmp = extension.substr(extension.size() - 5, extension.size());
	if(tmp != ".conf")
		return (false);
	return (true);
}

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
	std::vector<ParsConfig *>	_config;
	for (unsigned int i = 0; i < _nbrServer; i++)
	{
		ParsConfig *tmp = new ParsConfig(config_file, i);
		_config.push_back(tmp);
	}

	//Initialisation des Server.
	std::vector<Server *>		_server;
	for (unsigned int i = 0; i < _nbrServer; i++)
	{
		Server *tmp = new Server(*_config[i]);
		_server.push_back(tmp);
	}

	//Crate connection
	Connection	_connection(_server);

	while (42)
	{
		_connection.initConnection();
		_connection.runSelect();
		_connection.acceptSocket();
		_connection.traitement();
	}

    return 0;
}
