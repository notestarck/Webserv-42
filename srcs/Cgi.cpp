//
// Created by Stephane Walter on 4/20/23.
//
#include "../include/Cgi.hpp"
#include "../include/Client.hpp"
#include "../include/utils.hpp"


Cgi::Cgi(Client &client, ParsConfig::Location *location)
{
	_cgiScript = location->getCgiScript();
	_cgiPath = location->getCgiPath();
	_cgiBody = client._bodyReq.str();

	if (client._method == POST)
	{
		_envCgi["CONTENT_TYPE"] = client._headers["Content-Type"].c_str();  //content type """ a tester""
		_envCgi["CONTENT_LENGTH"] = client._contentLenght;
		for (int i = 0; _env[i]; i++)
		{
			std::string line;
			line = _env[i];
			size_t separator = line.find("=");
			if (separator != std::string::npos)
			{
				std::string headerName = line.substr(0, separator);
				std::string headerValue = line.substr(separator + 1, line.size() - separator - 1);
				// std::cout << headerName << " - " << headerValue << std::endl;
				_envCgi[headerName] = headerValue;
			}
		}
	}
	else
	{
		_envCgi["SERVER_PROTOCOL"] = client._httpVersion;
		_envCgi["SERVER_SOFTWARE"] = "Webserv";
		_envCgi["SERVER_PORT"] =    client._server.getPort();
		_envCgi["SERVER_NAME"] = client._server.getNameServer();
		_envCgi["REQUEST_METHOD"] = client._method;
		_envCgi["REDIR_STATUS"] = "200";
		_envCgi["GETEWAY_INTERFACE"] = "CGI/1.1";
		_envCgi["QUERY_STRING"] = client._query.substr(1, client._query.size());
	}
	_envCgi["HTTP_COOKIE"] = client._cookie;
}

Cgi::~Cgi() {}

Cgi::Cgi(const Cgi &srcs)
{
	*this = srcs;
}

Cgi &Cgi::operator=(Cgi const &src)
{
	if(this != &src)
	{
		_cgiPath   = src._cgiPath;

		_cgiScript = src._cgiScript;
		_cgiBody = src._cgiBody;
		_envCgi = src._envCgi;
	}
	return *this;
}

char **Cgi::getenv() const
{
	char **env = new char*[_envCgi.size() + 1];
	int i = 0;
	for(std::map<std::string, std::string>::const_iterator j = _envCgi.begin(); j != _envCgi.end(); j++) {
		std::string res = j->first + "=" + j->second;
		env[i] = new char[res.size() + 1];
		env[i] = strcpy(env[i], (const char*)res.c_str());
		i++;
	}
	env[i] = NULL;
	return env;
}

char **Cgi::arg(Client &client)
{
	char **argv;
	if (_cgiScript.find(".py") != std::string::npos)
	{
		std::string truc = client._bodyReq.str();
		std::string _login = truc.substr(truc.find('=') + 1);


		argv = new char *[3];
		argv[0] = new char[strlen(_cgiScript.c_str()) + 1];
		argv[1] = new char[strlen(_login.c_str()) + 1];

		std::strcpy(argv[0], _cgiScript.c_str());
		std::strcpy(argv[1], _login.c_str());

		argv[2] = 0;
	}
	else
	{
		argv = new char *[3];
		argv[0] = new char[_cgiPath.size() + 1];
		std::strcpy(argv[0], _cgiPath.c_str());

		argv[1] = new char[strlen(_cgiScript.c_str()) + 1];
		std::strcpy(argv[1], _cgiScript.c_str());
		argv[2] = 0;
	}

	return (argv);
}



