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
#include <sys/stat.h>
#include <fcntl.h>
#include <map>
#include <sstream>

#include "./Server.hpp"
#include "./Client.hpp"
#include "./Request.hpp"
#include "./Response.hpp"

#include <vector>
#define MAX_URI_SIZE 64
#define BSIZE 1024
enum MethodType{
    GET,
    POST,
    DELETE,
    UNKNOWN


};
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
		//void send_error(int i);

        //  clients
        bool dead_or_alive(Client client, bool alive = false);
        bool live_request(char *request) const;
        bool request_ok(char *request);
        bool live_request(std::map<std::string, std::string> *headers) const;
        void get_method(Client &client, std::string path);
        std::string find_path_in_root(std::string path, Client &client) const;
        std::string longToString(long number);
        const char *find_type(const char *path) const;
        void delete_method(Client client, std::string path);
        std::string methodtype(MethodType method) const;
        void send_error(int code, Client &client, std::vector<MethodType> *allow_methods);
	private :
		/* Serveur */
		std::vector<Server *>	_servers;
		std::vector<Client>		_client;
        int test;
        std::map<int, std::string> _status_info;

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