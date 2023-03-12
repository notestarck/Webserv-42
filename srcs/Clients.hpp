//
// Created by Stephane Walter on 3/6/23.
//

#ifndef WEBSERV_CLIENTS_HPP
#define WEBSERV_CLIENTS_HPP
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>
#include "Servers.hpp"

#define MAX_REQUEST_SIZE 2048

class Clients {

private:
    int socket;
    int rec_size;
    timeval last_get_time;
public:

    socklen_t address_length;
    struct sockaddr_storage address;
    char requet[MAX_REQUEST_SIZE + 1];

    ~Clients();
    Clients();
    Clients(Servers *server);
    Servers *server;

void set_socket(int val);
void set_rec_size(int size);

int get_socket() const;
int get_rec_size() const;

const char  *get_address() const;
const char *get_port() const;

void clear_requet();

};


#endif //WEBSERV_CLIENTS_HPP
