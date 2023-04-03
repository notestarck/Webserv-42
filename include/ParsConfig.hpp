/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsConfig.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/16 14:28:13 by estarck           #+#    #+#             */
/*   Updated: 2023/04/03 11:05:36 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _PARSCONFIG_HPP_
#define _PARSCONFIG_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <vector>
#include <map>

#define localhost "127.0.0.1"

class ParsConfig
{
	public :
		class Location
		{
			public :
				Location(std::ifstream &file_config, std::string url);
				Location(const Location &srcs);
				~Location();

				Location & operator=(const Location &srcs);

				const std::string &					getUrl() const;
				const std::vector<std::string> &	getAllow() const;
				const std::string &					getRoot() const;
				const std::string &					getIndex() const;

			private :
				Location();
				std::string 				_url;
				std::vector<std::string>	_allow;
				std::string 				_root;
				std::string 				_index;
				//ajouter le cgi_info
		};// class Locartion

		ParsConfig();
		ParsConfig(std::ifstream &file_config);
		ParsConfig(const ParsConfig &srcs);
		~ParsConfig();

		ParsConfig & operator=(const ParsConfig &srcs);

		//Getteur
		std::string              		getHost() const;
		unsigned int             		getPort() const;
		std::string              		getNameServer() const;
		std::string              		getRoot() const;
		std::string              		getIndex() const;
		std::string              		getErrorPage(int code) const;
		std::string              		getLocationUrl(size_t size) const;
		std::vector<std::string>		getLocationAllow(std::string url) const;
		std::string              		getLocationRoot(std::string url) const;
		std::string              		getLocationIndex(std::string url) const;
		const std::vector<Location>&	getLocation() const;
		size_t              			getNbrLocation() const;

	private :
		std::string              	_host;
		unsigned int           		_port;
		std::string             	_name_server;
		std::string            		_root;
		std::string           		_index;
		std::map<int, std::string>	_error_page;
		std::vector<Location>	    _location;
		size_t              		_nbrLocation;

		//Setteur
		void    setHost(std::string ip);
		void    setPort(unsigned int port);
		void    setNameServer(std::string nameServer);
		void    setRoot(std::string root);
		void    setIndex(std::string index);
		void    setErrorPage(int error, std::string page);
		void    setLocation(std::ifstream &file_config, std::string url);
};// class ParsConfig

#endif /* _PARSCONFIG_HPP_ */