/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 12:22:31 by estarck           #+#    #+#             */
/*   Updated: 2023/03/16 10:05:19 by estarck          ###   ########.fr       */
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

#include "../include/Master.hpp"
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
	
	//Creation du Master
	Master	my_webServ(config_file, nbr_server);
	
	return (0);
}
