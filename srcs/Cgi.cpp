//
// Created by Stephane Walter on 4/20/23.
//
#include "../include/Cgi.hpp"
#include "../include/Client.hpp"


Cgi::Cgi(Client &client)
{

	_envCgi["SERVER_PROTOCOL"] = client._httpVersion;
	_envCgi["SERVER_PORT"] =    client._server.getPort();
	_envCgi["REQUEST_METHOD"] = client._method;
	_envCgi["PATH_INFO"] = client._uri;
	_envCgi["PATH_TRANSLATED"] =     //chemin absolue du scritp
	_envCgi["SCRIPT_NAME"] =        // chemin d acces relatif
	_envCgi["QUERY_STRING"] =
	_envCgi["REMOTE_HOST"] =
	_envCgi["REMOTE_ADDR"] =
	_envCgi["AUTH_TYPE"] =
	_envCgi["REMOTE_USER"] =
	_envCgi["REMOTE_IDENT"] =
	_envCgi["CONTENT_TYPE"] =
	_envCgi["CONTENT_LENGTH"] =
	_envCgi["HTTP_ACCEPT"] =
	_envCgi["HTTP_ACCEPT_LANGUAGE"] =
	_envCgi["HTTP_USER_AGENT"] =
	_envCgi["HTTP_COOKIE"] = client._cookie;
	_envCgi["HTTP_REFERER"] =
}