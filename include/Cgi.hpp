//
// Created by Stephane Walter on 4/20/23.
//

#ifndef WEBSERV_42_CGI_HPP
#define WEBSERV_42_CGI_HPP
#include "Connection.hpp"



class Cgi{
public:
	Cgi();
	Cgi(Client &client, std::string gipqath);
	~Cgi();

	Cgi(const Cgi &srcs);
	Cgi &operator=(const Cgi &srcs);

	//std::string exec_cgi();




private:
	std::map<std::string, std::string>  _envCgi;
	std::stringstream                   _cgiBody;
	std::string                         _cgiPath;    // chemin du bin
	std::string                         _cgiScript;  // chemin du script cgi
	char                                **arg();
	char                                **getenv() const;



};



#endif //WEBSERV_42_CGI_HPP
