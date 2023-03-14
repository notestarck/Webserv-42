/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:50:28 by estarck           #+#    #+#             */
/*   Updated: 2023/03/14 15:29:29 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _MASTER_HPP_
#define _MASTER_HPP_

#include "./Client.hpp"
#include "./ParsConfig.hpp"
#include "./Server.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// @brief On pourrait l'appeler Connection, elle gere les clients, maintient
// une liste de clients connectés en enregistrant les retours de accept qui
// représente chaque client effectivement connecté à notre serveur,
// et en supprimant ceux dont le socket retourne une erreur indiquant qu'ils
// ont été déconnectés.
class Master
{
	public :
		Master(std::vector<Server *> &server);
		Master(const Master &srcs);
		~Master();

		Master &operator=(const Master &srcs);

		void initSelect();
		void initConnection();
		void checkClient();

	private :
		/* Serveur */
		std::vector<Server *>		_server;

		// @brief Garder en memoire les clients connectes.
		std::vector<Client>	_client;

		// @brief Gestion des fd, FD_ZERO pour l'initialiser et FD_SET pour mettre les valeurs des sockets.
		fd_set	_set;
		fd_set	_write;
		fd_set	_read;
		fd_set	_errors;
		// @brief Identifiant du descripteur le plus élevé
		int		_maxFd;
		
		// @brief est un pointeur vers une structure pour le temps maximum
		// que select doit attendre et bloquer avant de retourner, une valeur de
		// nullptr permet de bloquer jusqu'à ce qu'un des sockets soit prêt à
		// lire ou écrire.
		timeval	_timeout;

		
		
};//class Master

#endif /*_MASTER_HPP_*/