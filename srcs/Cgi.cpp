//
// Created by Stephane Walter on 4/20/23.
//
#include "../include/Cgi.hpp"
#include "../include/Client.hpp"


Cgi::Cgi(Client &client):
{
	_cgiBody << client._bodyReq;
	_envCgi["SERVER_PROTOCOL"] = client._httpVersion;
	_envCgi["SERVER_SOFTWARE"] = "Webserv";
	_envCgi["SERVER_PORT"] =    client._server.getPort();
	_envCgi["REQUEST_METHOD"] = client._method;
	_envCgi["PATH_INFO"] = client._uri;
	_envCgi["PATH_TRANSLATED"] = client._uri;     //chemin absolue du scritp
	_envCgi["SCRIPT_NAME"] = client._filePath;     // chemin d acces relatif
	_envCgi["QUERY_STRING"] = client._query;
	_envCgi["REDIR_STATUS"] = "200";   // pour cgi-php
	_envCgi["REMOTE_HOST"] = client._server.getHost();
	_envCgi["GETEWAY_INTERFACE"] = "CGI/1.1";
	_envCgi["REMOTE_ADDR"] =;
	_envCgi["AUTH_TYPE"] =;
	_envCgi["REMOTE_USER"] =;
	_envCgi["REMOTE_IDENT"] =;
	if (client._method = "POST") {
		_envCgi["CONTENT_TYPE"] = client._headers["Content-Type"].c_str();  //content type """ a tester""
		_envCgi["CONTENT_LENGTH"] = client._contentLenght;
	}
	_envCgi["HTTP_ACCEPT"] =;
	_envCgi["HTTP_ACCEPT_LANGUAGE"] =;
	_envCgi["HTTP_USER_AGENT"] =;
	_envCgi["HTTP_COOKIE"] = client._cookie;
	_envCgi["HTTP_REFERER"] =;
}

Cgi::~Cgi() {}

Cgi &Cgi::operator=(Cgi const &src){
	if(this != &src)
	{
		_cgiBody << src._cgiBody;
		_envCgi = src._envCgi;
	}
	return *this;
}

std::string Cgi::exec_cgi() {

}

char **Cgi::getenv() const{
	char **env = new char*[_envCgi.size() + 1];
	int i = 0;
	for(std::map<std::string, std::string>::const_iterator j = _envCgi.begin(); j != _envCgi.end()) {
		std::string res = j->first + "=" + j->second;
		env[i] = new char[res.size() + 1];
		env[i] = strcpy(env[i], (const char*)res.c_str());
		i++;
	}
	env[i] = NULL;
	return env;
}