//
// Created by Stephane Walter on 4/20/23.
//

#ifndef WEBSERV_42_CGI_HPP
#define WEBSERV_42_CGI_HPP

#include "Connection.hpp"
#include "../include/Client.hpp"
//#include "../include/utils.hpp"

class Cgi
{
	public:
		Cgi(Client &client, ParsConfig::Location *location);
		~Cgi();

		Cgi(const Cgi &srcs);
		Cgi &operator=(const Cgi &srcs);

		//std::string exec_cgi();
		char                                **arg(Client &client);
		char                                **getenv() const;

	private:
		Cgi();
		std::map<std::string, std::string>  _envCgi;
		std::stringstream                   _cgiBody;
		std::string                         _cgiPath;    // chemin du bin
		std::string                         _cgiScript;  // chemin du script cgi
};



#endif //WEBSERV_42_CGI_HPP
