/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:32:36 by estarck           #+#    #+#             */
/*   Updated: 2023/03/15 15:39:19 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include "Master.hpp"

struct Client
{
    /* Socket et contexte d'adressage du client */
	int					_csock;
	sockaddr_storage	_csin;
	socklen_t			_crecsize;
	timeval				_lastGetTime;
};//struct Client

#endif/* _CLIENT_HPP_ */