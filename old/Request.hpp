/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reclaire <reclaire@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/16 10:24:17 by estarck           #+#    #+#             */
/*   Updated: 2023/03/16 17:47:19 by reclaire         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _REQUEST_HPP_
#define _REQUEST_HPP_

#include <iostream>
#include <sstream>

#include <map>

enum MethodType{
    GET,
    POST,
    DELETE,
    UNKNOWN
};

class Request
{
	public:
		Request(int i);
		~Request();
	
	    std::string	get_path();

		int			parse(std::string request);
		bool		is_not_method(const std::string method);
		bool		check_protocol(std::map<std::string, std::string>::mapped_type &test);
	
		/// @brief Ressource de la requete http
		std::string							_path;
		
		/// @brief Methode de la requete
		std::string							_method;

		/// @brief map cles valeur du header
		std::map<std::string, std::string>	_headers;

		/// @brief media soit code html, data....
		std::string							_body;

		/// @brief taille de la requete
        size_t								_content_length;
	private:
		Request();

		int	client_fd;
};//Class Request

#endif/* _REQUEST_HPP_ */