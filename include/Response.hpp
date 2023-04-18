/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 15:48:44 by estarck           #+#    #+#             */
/*   Updated: 2023/04/18 11:38:58 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _RESPONSE_HPP_
#define _RESPONSE_HPP_

#include "./Client.hpp"

struct Client;

	void createHttpResponse(Client &client, int statusCode, const std::string &contentType);
	void sendHttpResponse(Client &client);
	void sendErrorResponse(Client &client, int code);

#endif /* _RESPONSE_HPP_ */