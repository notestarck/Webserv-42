//
// Created by Stephane Walter on 4/20/23.
//
#include "../include/Cgi.hpp"
#include "../include/Client.hpp"


Cgi::Cgi(Client &client, std::string cgipath)
{

	(void)cgipath;
//_cgiScript = "www/cgi/test.php";
//_cgiPath = "/usr/bin/php";

	_cgiScript = "www/cgi/cgi.py";
	_cgiPath = "/usr/bin/python3";
	_cgiBody << client._bodyReq;
	_envCgi["SERVER_PROTOCOL"] = client._httpVersion;
	_envCgi["SERVER_SOFTWARE"] = "Webserv";
	_envCgi["SERVER_PORT"] =    client._server.getPort();
	_envCgi["SERVER_NAME"] = "toto";
	_envCgi["REQUEST_METHOD"] = client._method;
	_envCgi["REQUEST_URI"] = _cgiPath;
	_envCgi["PATH_INFO"] = _cgiPath;
	_envCgi["PATH_TRANSLATED"] =  _cgiScript;     //chemin absolue du scritp
	_envCgi["SCRIPT_NAME"] = _cgiPath;     // chemin d acces relatif
	_envCgi["QUERY_STRING"] = client._query;
	_envCgi["REDIR_STATUS"] = "200";   // pour cgi-php
	//_envCgi["REMOTE_HOST"] = client._server.getHost();
	_envCgi["GETEWAY_INTERFACE"] = "CGI/1.1";
	_envCgi["REMOTE_ADDR"] = "1";
	//_envCgi["AUTH_TYPE"] = "1";
	//_envCgi["REMOTE_USER"] = "1";
	//_envCgi["REMOTE_IDENT"] = "1";
	if (client._method == POST) {
		_envCgi["CONTENT_TYPE"] = client._headers["Content-Type"].c_str();  //content type """ a tester""
		_envCgi["CONTENT_LENGTH"] = client._contentLenght;
	}
	//_envCgi["HTTP_ACCEPT"] = "1";
	//_envCgi["HTTP_ACCEPT_LANGUAGE"] = "1";
	//_envCgi["HTTP_USER_AGENT"] = "1";
	_envCgi["HTTP_COOKIE"] = client._cookie;
	//_envCgi["HTTP_REFERER"] = "1";


}

Cgi::~Cgi() {}

Cgi &Cgi::operator=(Cgi const &src){
	if(this != &src)
	{
		_cgiPath   = src._cgiPath;

		_cgiScript = src._cgiScript;
		_cgiBody << src._cgiBody;
		_envCgi = src._envCgi;
	}
	return *this;
}

//std::string Cgi::exec_cgi() {
//
//}

char **Cgi::getenv() const{
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

char **Cgi::arg(Client &client) {
	char **argv;
	std::cout << "je lance fonction arg\n";
	if (_cgiScript.find(".py") != std::string::npos) {
		std::string truc = client._bodyReq.str();
		std::string _login = truc.substr(truc.find('=') + 1);


		argv = new char *[3];
		argv[0] = new char[strlen(_cgiPath.c_str()) + 1];
		argv[1] = new char[strlen(_login.c_str()) + 1];

		std::strcpy(argv[0], _cgiPath.c_str());
		std::strcpy(argv[1], _login.c_str());

		argv[2] = 0;

	} else {
		argv = new char *[3];
		argv[0] = new char[_cgiPath.size() + 1];
		std::strcpy(argv[0], _cgiPath.c_str());

		argv[1] = new char[strlen(_cgiScript.c_str()) + 1];
		std::strcpy(argv[1], _cgiScript.c_str());
		argv[2] = 0;
	}

	return (argv);
}



