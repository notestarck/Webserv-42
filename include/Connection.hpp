/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 13:43:22 by estarck           #+#    #+#             */
/*   Updated: 2023/04/18 17:49:56 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

#include <sys/select.h>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <map>
#include <sstream>

#include "./Server.hpp"
#include "./Client.hpp"
#include "./HTTPRequest.hpp"
#include "./Response.hpp"
#include "./Autodindex.hpp"

#include <vector>
#define BSIZE 1024
#define MAX_URI_SIZE 64

class Connection
{
	public :
		Connection();
		Connection(std::vector<Server *> &servers);
		Connection(const Connection & srcs);
		~Connection();

		Connection & operator=(const Connection &srcs);

		//Initialisation de la connexion du serveur
		void initMime();
		void initConnection();
		void initSelect(int fd, fd_set &set);
		void runSelect();

		//Fonctionnement coté client
		Client *findExistingClient(const sockaddr_in& csin);
		void acceptSocket();
		void traitement();
		void clearClientSockets();

		// @brief lit la requete client et la parse, gere les erreurs
		// @param client client en cours de vecteur<Client> (*it)
		// @return true si client toujours en vie.
		bool receiveClientRequest(Client &client);

		/// @brief Check si la requete client est conforme
		/// @param request request du client a check.
		/// @return bool true si confrome
		bool checkRequest(char *request);

		/// @brief Gestion de la request parse afin de generer la reponse
		/// @param client 
		bool handleReponse(Client &client);

		/// @brief gestion de Get
		bool handleGET(Client& client);
		bool hanglGetLocation(Client &client);
		/// @brief gestion de Post
		void handlePOST(Client& client);
		/// @brief gestion de Delete
		void handleDELETE(Client& client);

		std::string getFilePath(const Client &client);
		std::string getFilePath(const Client &client, const ParsConfig::Location *location);
		std::string getMimeType(const std::string& filePath);
		ParsConfig::Location *findLocationForUri(const std::string& uri, const std::vector<ParsConfig::Location>& locations);
		void executeCGI(Client &client, const std::string &cgiPath);
		
		bool deadOrAlive(Client client, bool alive);

	private :
		/* Serveur */
		std::vector<Server *>		_servers;
		std::vector<Client>			_client;

		/* Gestion des fd */ //Gestion des fd, FD_ZERO pour l'initialiser et FD_SET pour mettre les valeurs des sockets.
		int		_maxFd;
		fd_set	_read;
		fd_set	_write;
		fd_set	_error;

		// @brief est un pointeur vers une structure pour le temps maximum
		// que select doit attendre et bloquer avant de retourner, une valeur de
		// nullptr permet de bloquer jusqu'à ce qu'un des sockets soit prêt à
		// lire ou écrire.
		timeval	_timeout;

		// Types mime
		std::map<std::string, std::string> _mimeTypes;
};//class Connection

#endif/* _CONNECTION_HPP_ */