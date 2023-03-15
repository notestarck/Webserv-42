//
// Created by stephane walter on 05/03/2023.
//

#ifndef WEBSERV_SERVERS_HPP
#define WEBSERV_SERVERS_HPP
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <cstdlib>
#include <iostream>
#include <arpa/inet.h>
#include <map>
#include <vector>


class Servers{
public:


private:
     std::string host;
     std::string port;


    std::string             	_name_server;
    std::string            		_root;
    std::string           		_index;
    std::map<int, std::string>	_error_page;
    //std::vector<Location *> loc;
    size_t              		_nbrLocation;

public:

    int listen_socket;


    struct timeval send_timeout;
    struct timeval recv_timeout;



    Servers();
    ~Servers();
    Servers(std::string host, std::string port);
    //Servers(ParsConfig &server);

    std::string getHost();

    //Servers(std::string host, int port);

    void create_socket();


};



#endif //WEBSERV_SERVERS_HPP
