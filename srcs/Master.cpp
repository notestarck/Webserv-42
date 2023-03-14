/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:55:07 by estarck           #+#    #+#             */
/*   Updated: 2023/03/14 18:11:01 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Master.hpp"

Master::Master(std::vector<Server *> &server) :
	_server(server) ,
	_maxFd(0)
{
	FD_ZERO(&_set);
	FD_ZERO(&_write);
	FD_ZERO(&_read);
	FD_ZERO(&_errors);
	_timeout.tv_sec = 1;
	_timeout.tv_usec = 0;
	initSelect();
	while (42)
	{
		initConnection();
		checkClient();
	}
}

Master::Master(const Master &srcs)
{}

Master::~Master()
{}

Master &Master::operator=(const Master &srcs)
{ return (*this); }

void Master::initSelect()
{
	for (std::vector<Server *>::iterator iter = _server.begin(); iter < _server.end(); iter++)
	{
		FD_SET((*iter)->getSocket(), &_set);
		if((*iter)->getSocket() > _maxFd)
			_maxFd = (*iter)->getSocket();
	}
}

void Master::initConnection()
{
	fd_set	tmp_set = _set;
	//Permet de récupérer le statut d'écriture, lecture ou erreur d'un ou plusieurs sockets cote serveur
	if (select(_maxFd + 1, &tmp_set, nullptr, nullptr, &_timeout) < 0)
	{
		std::cerr << "Error : Status of socket Master::acceptConnection()" << std::endl;
		exit(-7);
	}
	for (std::vector<Server *>::iterator iter = _server.begin(); iter < _server.end(); iter++)
	{
		if (FD_ISSET((*iter)->getSocket(), &tmp_set))
		{
			//Socket server est pret a etre lu
			sockaddr_in	tmp_csin = {0};
			socklen_t	tmp_crecsize = sizeof(tmp_csin);
			if((*iter)->hasCapacity())
			{
				int client_fd = accept((*iter)->getSocket(), (sockaddr *)&tmp_csin, &tmp_crecsize);
				if (client_fd < 0)
					std::cerr << "Failed to accept connection on port " << (*iter)->getPort() << std::endl;
				else
				{
					(*iter)->incrementCurrentConnection();
					Client	newClient;
					newClient._csin = tmp_csin;
					newClient._crecsize = tmp_crecsize;
					_client.push_back(newClient);
					std::cout << "Accepted connection on port " << (*iter)->getPort() << std::endl;
				}
			}
			else
			{
				std::cerr << "Connection limit reached on port " << (*iter)->getPort() << std::endl;
			}
		}
	}
}

//void Master::checkClient()
//{
//	int highestFd = 0;
//
//	for (std::vector<Client>::iterator iter = _client.begin(); iter < _client.end(); iter++)
//	{
//		FD_SET(iter->_csock, &_read);
//		FD_SET(iter->_csock, &_write);
//		FD_SET(iter->_csock, &_errors);
//		if (iter->_csock > highestFd)
//			highestFd = iter->_csock;
//	}
//	//Renvoie le nombre de descripteurs de fichier a etre lu/ecrits...
//	int selectResult = select(highestFd + 1, &_read, &_write, &_errors, &_timeout);
//	if (selectResult > 0)
//	{
//		//au moins un client a une action a executer
//		std::cout << "Un client a quelque chose a executer\n";
//	}
//	else if (selectResult == -1)
//	{
//		// erreur
//		std::cout << "Erreur select checkClient()\n";
//	}
//}


//La fonction checkClient() va parcourir la liste des clients connectés et vérifier
//s'ils ont des données à lire ou à écrire. Si un client a des données à lire,
//la fonction appelle la méthode receiveData() dans la classe Server correspondante
//pour traiter ces données. Si un client a des données à écrire, la fonction appelle
//la méthode sendData() dans la classe Server correspondante pour envoyer les données.
//Si un client a une erreur de socket, la fonction supprime le client de la liste et
//ferme la socket.
void Master::checkClient()
{
	fd_set tmp_read = _read;
    fd_set tmp_write = _write;
    fd_set tmp_errors = _errors;

    if (select(_maxFd + 1, &tmp_read, &tmp_write, &tmp_errors, &_timeout) < 0)
    {
        std::cerr << "Error: Status of socket Master::handleClients()" << std::endl;
        exit(-8);
    }

    // Traitement des clients prêts à être lus
    for (auto it = _client.begin(); it != _client.end();)
    {
			std::cout << "ici\n";
        Client &client = *it;
        int client_socket = client._csock;
        if (FD_ISSET(client_socket, &tmp_read))
        {
            char buffer[1024];
            int read_size = recv(client_socket, buffer, sizeof(buffer), 0);
            if (read_size == 0)
            {
                // Le client a fermé la connexion
                close(client_socket);
                std::cout << "Client " << inet_ntoa(client._csin.sin_addr) << ":" << ntohs(client._csin.sin_port) << " disconnected" << std::endl;
                it = _client.erase(it);
                continue;
            }
            else if (read_size < 0)
            {
                std::cerr << "Error: Failed to receive data from client " << inet_ntoa(client._csin.sin_addr) << ":" << ntohs(client._csin.sin_port) << std::endl;
                ++it;
                continue;
            }

            // Traitement des données reçues
            // ...

            ++it;
        }
        else
        {
            ++it;
        }
    }

    // Traitement des clients prêts à écrire
    // ...

    // Traitement des clients en erreur
    // ...
}