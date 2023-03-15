#include "../include/ParsConfig.hpp"
#include <sstream>

/*************************** Class ParsConfig ****************************/

ParsConfig::ParsConfig()
{}

ParsConfig::ParsConfig(std::ifstream &file_config, int indexServer) :
	_port(80),
	_host(localhost),
	_name_server("default_name"),
	_index("index.html"),
	_nbrLocation(0)
{
	std::string	line;

	while(getline(file_config, line) && line != "}")
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
			setIndex(value.substr(0, value.size() - 1));
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
		_host = srcs._host;
		_port = srcs._port;
		_name_server = srcs._name_server;
		_root = srcs._root;
		for (size_t i = 0; i < _error_page.size(); i++)
			_error_page.at(i) = srcs._error_page.at(i);
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
	{
		std::cerr << "\033[1;31mError page_error\033[0m" << std::endl;
		exit (-2);
	}
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

std::string	ParsConfig::getLocationAllow(std::string url) const
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
	int i = 0;
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
		ss >> value;
		if (key == "allow")
			_allow = value.substr(0, value.size() -1);
		else if (key == "root")
			_root = value.substr(0, value.size() - 1);
		else if (key == "index")
			_index = value.substr(0, value.size() - 1);
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
		_allow = srcs._allow;
		_root = srcs._root;
		_index = srcs._index;
	}
	return (*this);
}

const std::string & ParsConfig::Location::getUrl() const
{ return (_url); }

const std::string & ParsConfig::Location::getAllow() const
{ return (_allow); }

const std::string & ParsConfig::Location::getRoot() const
{ return (_root); }

const std::string & ParsConfig::Location::getIndex() const
{ return (_index); }