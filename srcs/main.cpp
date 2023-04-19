/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 12:22:31 by estarck           #+#    #+#             */
/*   Updated: 2023/04/19 15:20:05 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/utils.hpp"

std::vector<Server *>		_server;
std::vector<ParsConfig *>	_config;
Connection					_connection;
volatile bool				boolStart = 1;
char						**_env;

void	creatFileDeleteMethod()
{
	std::ofstream outFile;
    for (int i = 1; i <= 4; ++i)
	{
        std::stringstream fileName;
        fileName << "fichier" << i << ".txt";

        outFile.open("./www/a_supprimer/" + fileName.str(), std::ios::binary | std::ios::out);
        if (outFile.is_open())
		{
            outFile << "Contenu du fichier " << i << std::endl;
            outFile.close();
        }
		else
            std::cerr << "Impossible de créer le fichier " << fileName.str() << std::endl;
    }
}

int main(int argc, char ** argv, char **env)
{
	signal(SIGINT, signal_handler);
	std::string  config_file_path;
	_env = env;

	//On verifie la presence du fichier .conf
	if (argc > 2)
	{
		std::cerr << "\033[1;31mError - Need  one argument : < nginx_config_file >\033[0m" << std::endl;
		exit (-1);
	}
	
	//On met un fichier par default s'il y'a pas de fichier config
	if (argc == 1)
	{
		std::cout << "\033[33mUtilisation d'un fichier .conf par default.\033[0m" << std::endl;
		config_file_path = "default.conf";
	}
	else
	{
		//On verifie l'extension du fichier.
		std::string	extension(argv[1]);
		if (!checkExtension(extension))
		{
			std::cerr << "\033[1;31mError : Need extension .conf\033[0m" << std::endl;
			exit (-1);
		}
		//Ouverture du fichier
		config_file_path = argv[1];
	}

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
		Server *tmp = new Server(_config[i]);
		_server.push_back(tmp);
	}

	//On cree les fichiers necessaire pour le tester de la methode DELETE
	creatFileDeleteMethod();

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
