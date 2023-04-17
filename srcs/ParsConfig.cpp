/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsConfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 17:38:12 by estarck           #+#    #+#             */
/*   Updated: 2023/04/14 11:49:49 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ParsConfig.hpp"

/*************************** Class ParsConfig ****************************/

ParsConfig::ParsConfig()
{}

ParsConfig::ParsConfig(std::ifstream &file_config) :
	_host(localhost),
	_port(80),
	_name_server("default_name"),
	_index("index.html"),
	_nbrLocation(0)
{
	std::string	line;

	while(std::getline(file_config, line) && line != "}")
	{
		std::string	key;
		std::string	value;
		std::stringstream ss;

		if (line.empty())
			continue;
        ss << line;
		ss >> key;
		ss >> value;
		if (key == "listen")
			setPort(stoi(value.substr(0, value.size() - 1)));
		else if (key == "host")
			setHost(value.substr(0, value.size() - 1));
		else if (key == "server_name")
			setNameServer(value.substr(0, value.size() - 1));
		else if (key == "root")
			setRoot(value.substr(0, value.size() - 1));
		else if (key == "index")
		{
			_index = (value.substr(0, value.size() - 1));
		}
		else if (key == "error_page")
		{
			std::string s_value;
			ss >> s_value;
			setErrorPage(stoi(value), s_value.substr(0, s_value.size() - 1));
		}
		else if (key == "location")
			setLocation(file_config, value);
		line.clear();
		ss.clear();
		key.clear();
		value.clear();
	}
}

ParsConfig::ParsConfig(const ParsConfig &srcs)
{ *this = srcs; }

ParsConfig::~ParsConfig()
{}

ParsConfig & ParsConfig::operator=(const ParsConfig &srcs)
{
	if (this != &srcs)
	{
		std::cout << "Constructeur d'operateur= de ParsConfig\n";
		_host = srcs._host;
		_port = srcs._port;
		_name_server = srcs._name_server;
		_root = srcs._root;
        _index = srcs._index;
		for (std::map<int, std::string>::iterator it = _error_page.begin(); it != _error_page.end(); it++)
			_error_page.insert(*it);
		for (size_t i = 0; i < srcs._location.size(); i++)
			_location.push_back(srcs._location[i]);
		_nbrLocation = srcs._nbrLocation;
	}
	return (*this);
}

unsigned int	ParsConfig::getPort() const
{ return (_port); }

std::string    ParsConfig::getHost() const
{ return (_host); }

std::string          ParsConfig::getNameServer() const
{ return (_name_server); }

std::string          ParsConfig::getRoot() const
{ return (_root); }

std::string			ParsConfig::getIndex() const
{ return (_index); }

std::string    ParsConfig::getErrorPage(int code) const
{
	std::map<int, std::string>::const_iterator	it = _error_page.find(code);
	if (it == _error_page.end())
		return("notFound");
	return (it->second);
}

std::string	ParsConfig::getLocationUrl(size_t pos) const
{
	if (pos >= _location.size())
	{
		std::cerr << "\033[1;31mgetLocationUrl : Bad size !\033[0m" << std::endl;
		exit (-4);
	} 
	return (_location[pos].getUrl()); }

std::vector<std::string>	ParsConfig::getLocationAllow(std::string url) const
{
	for(std::vector<Location>::const_iterator it = _location.begin(); it != _location.end(); it++)
	{
		if(it->getUrl() == url)
			return (it->getAllow());
	}
	std::cerr << "\033[1;31mgetLocationAllow : url don't exist ! \033[0m" << std::endl;
	exit (-3);
}

std::string	ParsConfig::getLocationRoot(std::string url) const
{
	for(std::vector<Location>::const_iterator it = _location.begin(); it != _location.end(); it++)
	{
		if(it->getUrl() == url)
			return (it->getRoot());
	}
	std::cerr << "\033[1;31mgetLocationRoot : url don't exist ! \033[0m" << std::endl;
	exit (-3);
}

std::string	ParsConfig::getLocationIndex(std::string url) const
{
	for(std::vector<Location>::const_iterator it = _location.begin(); it != _location.end(); it++)
	{
		if(it->getUrl() == url)
			return (it->getIndex());
	}
	std::cerr << "\033[1;31mgetLocationIndex : url don't exist ! \033[0m" << std::endl;
	exit (-3);
}

size_t	ParsConfig::getNbrLocation() const
{ return (_nbrLocation); }

std::vector<ParsConfig::Location> &ParsConfig::getLocation()
{ return (_location); }

void    ParsConfig::setHost(std::string ip)
{
	if (ip == "localhost")
	{
		_host = localhost;
	}
	else
		_host = ip;
}

void    ParsConfig::setPort(unsigned int port)
{ _port = port; }

void    ParsConfig::setNameServer(std::string nameServer)
{ _name_server = nameServer; }

void    ParsConfig::setRoot(std::string root)
{ _root = root; }

void	ParsConfig::setIndex(std::string index)
{ _index = index; }

void    ParsConfig::setErrorPage(int error, std::string page)
{ _error_page.insert(make_pair(error, page)); }

void    ParsConfig::setLocation(std::ifstream &file_config, std::string url)
{
	size_t i = 0;
	Location tmp(file_config, url);

	while (i != _location.size() && _location[i].getUrl() != url)
		i++;
	if (i != _location.size())
		_location[i] = tmp;
	else
	{
		std::vector<Location>::const_iterator it = _location.end();
		_location.insert(it, tmp);
		_nbrLocation += 1;
	}
}


/*************************** Class Location ****************************/

ParsConfig::Location::Location(std::ifstream &file_config, std::string url) :
	_url(url)
{
	std::string			line;

	while(getline(file_config, line))
	{
		std::stringstream	ss;
		std::string			key;
		std::string			value;

		ss << line;
		if (line.empty())
			continue;
		ss >> key;
		if (key == "}")
			break;
		while (ss >> value)
		{
			if (key == "allow")
			{
				std::string	tmp;
				if (!ss.eof())
					tmp = value.substr(0, value.size());
				else
					tmp = value.substr(0, value.size() -1);
				if (tmp != "GET" && tmp != "POST" && tmp != "DELETE")
				{
					std::cerr << "Error : file config not valide. " << tmp << " is not valide." << std::endl;
					exit (-1);
				}
				_allow.push_back(tmp);
			}
			else if (key == "root")
				_root = value.substr(0, value.size() - 1);
			else if (key == "index")
				_index = value.substr(0, value.size() - 1);
            else if(key == "cgi_pass")
                _cgiPath = value.substr(0, value.size() - 1);
		}
		line.clear();
		ss.clear();
		key.clear();
		value.clear();
	}
}

ParsConfig::Location::Location(const Location &srcs)
{ *this = srcs; }

ParsConfig::Location::~Location()
{}

ParsConfig::Location & ParsConfig::Location::operator=(const Location & srcs)
{
	if (this != &srcs)
	{
		_url = srcs._url;
		for (std::vector<std::string>::const_iterator it = srcs._allow.begin(); it < srcs._allow.end(); it++)
			_allow.push_back(*it);
		_root = srcs._root;
		_index = srcs._index;
        _cgiPath = srcs._cgiPath;
	}
	return (*this);
}

const std::string & ParsConfig::Location::getUrl() const
{ return (_url); }

const std::vector<std::string> & ParsConfig::Location::getAllow() const
{ return (_allow); }

const std::string & ParsConfig::Location::getRoot() const
{ return (_root); }

const std::string & ParsConfig::Location::getIndex() const
{ return (_index); }

const std::string & ParsConfig::Location::getCgiPath() const
{ return (_cgiPath); }

bool ParsConfig::Location::isMethodAllowed(std::string method) const
{
	for (std::vector<std::string>::const_iterator it = _allow.begin(); it != _allow.end(); it++)
	{
		if (*it == method)
			return (true);
	}
	return (false);
}