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

class Servers{
public:


private:
     std::string host;
     std::string port;
public:

    int listen_socket;


    struct timeval send_timeout;
    struct timeval recv_timeout;



    Servers();
    ~Servers();
    Servers(std::string host, std::string port);

    std::string getHost();

    //Servers(std::string host, int port);

    void create_socket();


};



#endif //WEBSERV_SERVERS_HPP
