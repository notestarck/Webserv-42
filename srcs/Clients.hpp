//
// Created by Stephane Walter on 3/6/23.
//

#ifndef WEBSERV_CLIENTS_HPP
#define WEBSERV_CLIENTS_HPP
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include "Servers.hpp"


class Clients {

private:

public:

    socklen_t address_length;
    struct sockaddr_storage address;
    int socket;

    ~Clients();
    Clients();
    Clients(Servers *server);
    Servers *server;

void set_socket(int val);

int get_socket() const;

const char  *get_address() const;

void request();

};


#endif //WEBSERV_CLIENTS_HPP
