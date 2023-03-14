/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:50:28 by estarck           #+#    #+#             */
/*   Updated: 2023/03/14 11:08:54 by estarck          ###   ########.fr       */
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
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// @brief On pourrait l'appeler Connection, elle gere les clients, maintient
// une liste de clients connectés en enregistrant les retours de accept qui
// représente chaque client effectivement connecté à notre serveur,
// et en supprimant ceux dont le socket retourne une erreur indiquant qu'ils
// ont été déconnectés.
class Master
{
	public :
		Master();
		Master(const Master &srcs);
		~Master();

		Master &operator=(const Master &srcs);

		

	private :
		// @brief Garder en memoire les clients connectes.
		std::vector<Client>	_client;

		// @brief Gestion des fd, FD_ZERO pour l'initialiser et FD_SET pour mettre les valeurs des sockets.
		fd_set	_set;
		
		// @brief est un pointeur vers une structure pour le temps maximum
		// que select doit attendre et bloquer avant de retourner, une valeur de
		// nullptr permet de bloquer jusqu'à ce qu'un des sockets soit prêt à
		// lire ou écrire.
		timeval	_timeout;

		
		
};//class Master

#endif /*_MASTER_HPP_*/