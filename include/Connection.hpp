/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 13:43:22 by estarck           #+#    #+#             */
/*   Updated: 2023/03/17 14:26:46 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

#include <sys/select.h>
#include <iostream>

#include "./Server.hpp"
#include "./Client.hpp"
#include "./Request.hpp"

#include <vector>

class Connection
{
	public :
		Connection();
		Connection(std::vector<Server *> &servers);
		Connection(const Connection & srcs);
		~Connection();

		Connection & operator=(const Connection &srcs);

		void initConnection();
		void initSelect(int fd, fd_set &set);
		void runSelect();
		void acceptSocket();
		void traitement();
		void send_error(int i);

	private :
		/* Serveur */
		std::vector<Server *>	_servers;
		std::vector<Client>		_client;

		/* Gestion des fd */ //Gestion des fd, FD_ZERO pour l'initialiser et FD_SET pour mettre les valeurs des sockets.
		int		_maxFd;
		fd_set	_read;
		fd_set	_write;
		fd_set	_errors;

		// @brief est un pointeur vers une structure pour le temps maximum
		// que select doit attendre et bloquer avant de retourner, une valeur de
		// nullptr permet de bloquer jusqu'à ce qu'un des sockets soit prêt à
		// lire ou écrire.
		timeval	_timeout;
};//class Connection

#endif/* _CONNECTION_HPP_ */