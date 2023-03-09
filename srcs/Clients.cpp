


//
// Created by Stephane Walter on 3/6/23.
//

#include "Clients.hpp"

Clients::~Clients(){}

Clients::Clients(Servers *server){
    this->server = server;
    address_length = sizeof(struct sockaddr_storage);

}

void Clients::set_socket(int val){
    socket = val;
    //std::cout << "debut set socket\n";
    fcntl(socket, F_SETFL, O_NONBLOCK);
    if(setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (void*)&server->recv_timeout, sizeof(struct timeval)) < 0)
        std::cout << "setsockopt recv timeout\n";
    if(setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (void*)&server->send_timeout, sizeof(struct timeval) ) < 0)
        std::cout << "setsockopt send timeout\n";

}

int Clients::get_socket() const{
    return socket;

}

const char  *Clients::get_address() const {
    // http://manpagesfr.free.fr/man/man3/getnameinfo.3.html

    static char buffer[50];
//    getnameinfo((struct sockaddr *)&address, address_length, 0, 0, buffer, sizeof(buffer), NI_NUMERICHOST);
//    return buffer;


    static char sbuf[50];

    if (getnameinfo((struct sockaddr *)&address, address_length,buffer, sizeof(buffer), sbuf,
                    sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) == 0)
        printf("host=%s, serv=%s\n", buffer, sbuf);

    return "1";

}