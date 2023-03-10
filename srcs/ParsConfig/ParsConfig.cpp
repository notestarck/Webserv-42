#include "../../include/ParsConfig/ParsConfig.hpp"
#include <sstream>

/*************************** Class ParsConfig ****************************/

ParsConfig::ParsConfig(ifstream &file_config, int indexServer) :
	_port(80),
	_ip(localhost),
	_name_server("default_name"),
	_index("index.html"),
	_nbrLocation(0)
{
	string	line;

	while(getline(file_config, line) && line != "}")
	{
		string	key;
		string	value;
		stringstream ss;

		if (line.empty())
			continue;
        ss << line;
		ss >> key;
		ss >> value;
		if (key == "listen")
			setPort(stoi(value.substr(0, value.size() - 1)));
		else if (key == "host")
			setIp(value.substr(0, value.size() - 1));
		else if (key == "server_name")
			setNameServer(value.substr(0, value.size() - 1));
		else if (key == "root")
			setRoot(value.substr(0, value.size() - 1));
		else if (key == "index")
			setIndex(value.substr(0, value.size() - 1));
		else if (key == "error_page")
		{
			string s_value;
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
{
	for (size_t i = 0; i < _nbrLocation; i++)
	{
    	delete _location[i];
    	_location[i] = nullptr;
	}
}

ParsConfig & ParsConfig::operator=(const ParsConfig &srcs)
{
	if (this != &srcs)
	{
		_ip = srcs._ip;
		_port = srcs._port;
		_name_server = srcs._name_server;
		_root = srcs._root;
		for (size_t i = 0; i < _error_page.size(); i++)
			_error_page.at(i) = srcs._error_page.at(i);
		for (size_t i = 0; i < _nbrLocation; i++)
		{
    		delete _location[i];
    		_location[i] = nullptr;
		}
		_location = srcs._location;
		_nbrLocation = srcs._nbrLocation;
	}
	return (*this);
}

unsigned int	ParsConfig::getPort() const
{ return (_port); }

string    ParsConfig::getIp() const
{ return (_ip); }

string          ParsConfig::getNameServer() const
{ return (_name_server); }

string          ParsConfig::getRoot() const
{ return (_root); }

string			ParsConfig::getIndex() const
{ return (_index); }

string    ParsConfig::getErrorPage(int code) const
{
	map<int, string>::const_iterator	it = _error_page.find(code);
	if (it == _error_page.end())
	{
		cerr << "\033[1;31mError page_error\033[0m" << endl;
		exit (-2);
	}
	return (it->second);
}

string	ParsConfig::getLocationUrl(size_t size) const
{
	if (size >= _location.size())
	{
		cerr << "\033[1;31mgetLocationUrl : Bad size !\033[0m" << endl;
		exit (-4);
	} 
	return (_location[size]->getUrl()); }

string	ParsConfig::getLocationAllow(string url) const
{
	for(int i = 0; i != _location.size(); i++)
	{
		if(_location[i]->getUrl() == url)
			return (_location[i]->getAllow());
	}
	cerr << "\033[1;31mgetLocationAllow : url don't exist ! \033\[0m" << endl;
	exit (-3);
}

string	ParsConfig::getLocationRoot(string url) const
{
	for(int i = 0; i != _location.size(); i++)
	{
		if(_location[i]->getUrl() == url)
			return (_location[i]->getRoot());
	}
	cerr << "\033[1;31mgetLocationRoot : url don't exist ! \033\[0m" << endl;
	exit (-3);
}

string	ParsConfig::getLocationIndex(string url) const
{
	for(int i = 0; i != _location.size(); i++)
	{
		if(_location[i]->getUrl() == url)
			return (_location[i]->getIndex());
	}
	cerr << "\033[1;31mgetLocationIndex : url don't exist ! \033\[0m" << endl;
	exit (-3);
}

size_t	ParsConfig::getNbrLocation() const
{ return (_nbrLocation); }

void    ParsConfig::setIp(string ip)
{
	if (ip == "localhost")
	{
		_ip = localhost;
	}
	else
		_ip = ip;
}

void    ParsConfig::setPort(unsigned int port)
{ _port = port; }

void    ParsConfig::setNameServer(string nameServer)
{ _name_server = nameServer; }

void    ParsConfig::setRoot(string root)
{ _root = root; }

void	ParsConfig::setIndex(string index)
{ _index = index; }

void    ParsConfig::setErrorPage(int error, string page)
{ _error_page.insert(make_pair(error, page)); }

void    ParsConfig::setLocation(ifstream &file_config, string url)
{
	int i = 0;
	Location *tmp = new Location(file_config, url);

	while (i != _location.size() &&_location[i]->getUrl() != url)
		i++;
	if (i != _location.size())
	{
		delete _location[i];
    	_location[i] = nullptr;
		_location[i] = tmp;
	}
	else
	{
		_location.push_back(tmp);
		_nbrLocation += 1;
	}
}


/*************************** Class Location ****************************/

ParsConfig::Location::Location(ifstream &file_config, string url) :
	_url(url)
{
	string			line;

	while(getline(file_config, line))
	{
		stringstream	ss;
		string			key;
		string			value;

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
		_allow = srcs._allow;
		_root = srcs._root;
		_index = srcs._index;
	}
	return (*this);
}

const string & ParsConfig::Location::getUrl() const
{ return (_url); }

const string & ParsConfig::Location::getAllow() const
{ return (_allow); }

const string & ParsConfig::Location::getRoot() const
{ return (_root); }

const string & ParsConfig::Location::getIndex() const
{ return (_index); }