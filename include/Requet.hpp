/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requet.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/16 10:24:17 by estarck           #+#    #+#             */
/*   Updated: 2023/03/16 14:44:49 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _REQUET_HPP_
#define _REQUET_HPP_

#include <iostream>
#include <sstream>

#include <map>

class Requet
{
	public:
		std::string path;
		std::string method;
		std::map<std::string, std::string> headers;
		std::string body;

		Requet(int i);
		~Requet();
	
		int parse(std::string request);
		bool is_not_method(const std::string method);
	
		bool check_protocol(std::map<std::string, std::string>::mapped_type &test);

	private:
		int client_fd;
		Requet();
	
};//Class Requet

#endif/* _REQUET_HPP_ */