#include "../../include/ParsConfig/ParsConfig.hpp"
#include <sstream>

/*************************** Class ParsConfig ****************************/

ParsConfig::ParsConfig(ifstream &file_config, int indexServer) :
	_ip(localhost)
{
	string	line;
	string	key;
	string	value;
	stringstream ss;

	while(getline(file_config, line) && line != "}")
	{
		ss.clear();
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
		else if (key == "error_page")
		{
			string s_value;
			ss >> s_value;
			setErrorPage(stoi(value), s_value.substr(0, s_value.size() - 1));
		}
		else if (key == "location")
			setLocation(value);
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

unsigned int	ParsConfig::getPort() const
{ return (_port); }

string    ParsConfig::getIp() const
{ return (_ip); }

string          ParsConfig::getNameServer() const
{ return (_name_server); }

string          ParsConfig::getRoot() const
{ return (_root); }

string    ParsConfig::getErrorPage(int code) const
{
	map<int, string>::const_iterator	it = _error_page.find(code);
	if (it == _error_page.end())
	{
		cerr << "Error page_error" << endl;
		exit (-2);
	}
	return (it->second);
}

//vector<Location *>	ParsConfig::getLocation() const
//{ return (&_location); }


void    ParsConfig::setIp(string ip)
{ _ip = ip; }

void    ParsConfig::setPort(unsigned int port)
{ _port = port; }

void    ParsConfig::setNameServer(string nameServer)
{ _name_server = nameServer; }

void    ParsConfig::setRoot(string root)
{ _root = root; }

void    ParsConfig::setErrorPage(int error, string page)
{ 
	_error_page.insert(make_pair(error, page)); }

void    ParsConfig::setLocation(string url)
{ 
	//cout << "Location : " << url << endl;
}


/*************************** Class Location ****************************/

ParsConfig::Location::Location(string url, string allow, string root, string index) :
	_url(url),
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