/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 12:22:31 by estarck           #+#    #+#             */
/*   Updated: 2023/03/15 18:54:13 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

#include "../include/Client.hpp"
#include "../include/Connection.hpp"
#include "../include/ParsConfig.hpp"
#include "../include/Server.hpp"


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

int main(int argc, char ** argv)
{
	//On verifie la presence du fichier .conf, il faut encore gerer l'extension du fichier.
	if (argc < 2)
	{
		std::cerr << "\033[1;31mError : Need < nginx_config_file >\033[0m" << std::endl;
		exit (-1);
	}

	//Oouverture du fichier
	const std::string  config_file_path(argv[1]);
	std::ifstream config_file(config_file_path.c_str());
	if (!config_file.is_open())
		std::cerr << "\033[1;31mError : Opening " << config_file_path << "\003[0m" << std::endl;

	//On compte le nombre de serveur qu'on enregistre ensuite dans my_config.
	unsigned int nbr_server = countServer(config_file);
	std::vector<ParsConfig *>  my_config;
	for (unsigned int i = 0; i < nbr_server; i++)
	{
		ParsConfig  *tmp = new ParsConfig(config_file, i);
		my_config.push_back(tmp);
	}

	//Creation des serveurs a parti du fichier de config.
	std::vector<Server *>  my_server;
	for (int i = 0; i < nbr_server; i++)
	{
	    Server *tmp = new Server(*my_config[i]);
	    my_server.push_back(tmp);
	}
	
	//Creation de la connexion
	Connection	my_connect(my_server);
	
		//delete my_server<Server *>
	for (int i = 0; i < nbr_server; i++)
		delete my_server[i];

	//delete my_config<parsconfig *>
	for (int i = 0; i < nbr_server; i++)
		delete my_config[i];
	
	return (0);
}