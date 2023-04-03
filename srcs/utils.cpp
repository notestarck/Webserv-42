/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/17 15:02:57 by estarck           #+#    #+#             */
/*   Updated: 2023/04/03 11:37:38 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/utils.hpp"

void	delProg()
{
	//delete _server
	std::cout << "\033[34mDelete des serveurs : \033[0m\n";
	for (size_t i = 0; i < _server.size(); i++) {
        close(_server[i]->getSocket());
        delete _server[i];
    }
	//delete _config
	std::cout << "\033[34mDelete de la config : \033[0m\n";
	for (size_t i = 0; i < _config.size(); i++)
		delete _config[i];
}

void signal_handler(int signal)
{
    std::cout << "Signal SIGINT reçu. Fermeture du programme..." << std::endl;
    if (signal == SIGINT)
	{
		delProg();
    	exit(1);
	}
}

int countServer(std::ifstream &config_file)
{
	int     nbr_server = 0;

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