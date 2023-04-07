/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 15:48:44 by estarck           #+#    #+#             */
/*   Updated: 2023/04/06 23:36:06 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _RESPONSE_HPP_
#define _RESPONSE_HPP_

#include "./Client.hpp"

struct Client;

// @brief Envoi une reponse HTTP
// @param client client destinataire de la reponse
// @param code code a renvoyer
void sendErrorResponse(Client &client, int code);

/// @brief 
/// @param client 
/// @param statusCode 
/// @param contentType 
/// @param body 
void sendHttpResponse(Client &client, int statusCode, const std::string &contentType, const std::string &body);

#endif /* _RESPONSE_HPP_ */