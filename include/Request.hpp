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

class Request
{
	public:
		std::string path;
		std::string method;
		std::map<std::string, std::string> headers;
		std::string body;

		Request(int i);
		~Request();
	
		int parse(std::string request);
		bool is_not_method(const std::string method);
	
		bool check_protocol(std::map<std::string, std::string>::mapped_type &test);

	private:
		int client_fd;
		Request();
	
};//Class Request

#endif/* _REQUEST_HPP_ */