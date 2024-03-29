/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/17 15:02:57 by estarck           #+#    #+#             */
/*   Updated: 2023/05/03 12:03:23 by estarck          ###   ########.fr       */
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
    if (signal == SIGINT)
	{
    	std::cout << "Signal SIGINT reçu. Fermeture du programme..." << std::endl;
		boolStart = 0;
	}
	if (signal == SIGPIPE)
	{
		std::cout << "Signal SIGPIPE recu. Fermeture du programme..." << std::endl;
		boolStart = 0;
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
	if (extension.size() < 5)
		return (false);
	std::string tmp = extension.substr(extension.size() - 5, extension.size());
	if(tmp != ".conf")
		return (false);
	return (true);
}

