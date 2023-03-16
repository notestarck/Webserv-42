/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:32:36 by estarck           #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2023/03/15 18:58:24 by estarck          ###   ########.fr       */
=======
/*   Updated: 2023/03/16 14:09:44 by estarck          ###   ########.fr       */
>>>>>>> master
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#define MAX_REQUEST_SIZE 2048

struct Client
{
    /* Socket et contexte d'adressage du client */
	int					_csock;
	sockaddr_storage	_csin;
	socklen_t			_crecsize;
	timeval				_lastGetTime;

	char				_recBuffer[MAX_REQUEST_SIZE + 1];
	int					_recSize;
};//struct Client

#endif/* _CLIENT_HPP_ */