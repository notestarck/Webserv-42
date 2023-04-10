/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 12:27:09 by estarck           #+#    #+#             */
/*   Updated: 2023/04/10 14:28:02 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HTTPREQUEST_HPP_
#define _HTTPREQUEST_HPP_

#include <string>
#include <sstream>

#include "Client.hpp"

#include <map>

struct Client;

class HTTPRequest
{
	public:
		HTTPRequest();
    	HTTPRequest(Client &client);
		HTTPRequest(const HTTPRequest &srcs);
		~HTTPRequest();

		const HTTPRequest &operator=(const HTTPRequest &srcs);

		// @brief Parse la request HTTP recu dans le client
		// @return la requete parse
		void		parseRequest(Client &client);
		
	private :

};

#endif /* _HTTPREQUEST_HPP_ */