//
// Created by Stephane Walter on 4/20/23.
//

#ifndef WEBSERV_42_CGI_HPP
#define WEBSERV_42_CGI_HPP
#include "Connection.hpp"



class Client;
class Cgi{
public:
	Cgi();
	Cgi(Client &client);
	~Cgi();

	Cgi(const Cgi &srcs);
	Cgi &operator=(const Cgi &srcs);

	std::string exec_cgi();

private:
	std::map<std::string, std::string>  _envCgi;




};



#endif //WEBSERV_42_CGI_HPP
