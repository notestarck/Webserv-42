#include "../../include/ParsConfig/ParsConfig.hpp"

/*************************** Class ParsConfig ****************************/

ParsConfig::ParsConfig(ifstream &file_config, int indexServer) :
	_ip(localhost)
{
	string 	line;
	string	key;
	string	value;

	while(getline(file_config, line) && line != "}")
	{
		file_config >> key;
		file_config >> value;
		if (key == "listen")
			setPort(stoi(value.substr(0, value.size() - 1)));
			//setPort(stoi(line.substr(key.size() + 1)));
		//else if (key == "host")
		//	setIp();
		//else if (key == "server_name")
		//	setNameServer();
		//else if (key == "root")
		//	setRoot();
		//else if (key == "error_page")
		//	setIp();
		//else if (key == "location")
		//	setIp();
	}
}

ParsConfig::ParsConfig(const ParsConfig &srcs)
{}

ParsConfig::~ParsConfig()
{}

ParsConfig & ParsConfig::operator=(const ParsConfig &srcs)
{
	//delete pour la copie
	return (*this);
}

unsigned int ParsConfig::getPort() const
{ return (_port); }

void    ParsConfig::setIp(string ip)
{}

void    ParsConfig::setPort(unsigned int port)
{
	_port = port;
}

void    ParsConfig::setNameServer(string nameServer)
{}

void    ParsConfig::setRoot(string root)
{}


/*************************** Class Location ****************************/

ParsConfig::Location::Location(string allow, string root, string index) :
	_allow(allow),
	_root(root),
	_index(index)
{}

ParsConfig::Location::Location(const Location &srcs)
{
	*this = srcs;
}

ParsConfig::Location::~Location()
{}

ParsConfig::Location & ParsConfig::Location::operator=(const Location & srcs)
{
	_allow = srcs._allow;
	_root = srcs._root;
	_index = srcs._index;
	return (*this);
}

const string & ParsConfig::Location::getAllow() const
{ return (_allow); }

const string & ParsConfig::Location::getRoot() const
{ return (_root); }

const string & ParsConfig::Location::getIndex() const
{ return (_index); }