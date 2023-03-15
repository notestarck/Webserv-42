


//
// Created by Stephane Walter on 3/6/23.
//

#include "Clients.hpp"

Clients::~Clients(){}

Clients::Clients(Servers *server){
    this->server = server;
    address_length = sizeof(struct sockaddr_storage);
    clear_requet();
    gettimeofday(&last_get_time, NULL);
    memset(requet, 0, MAX_REQUEST_SIZE + 1 );


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
    getnameinfo((struct sockaddr *)&address, address_length,  buffer, sizeof(buffer),0, 0, NI_NUMERICHOST);
    return buffer;


//    static char sbuf[50];
//
//    if (getnameinfo((struct sockaddr *)&address, address_length,buffer, sizeof(buffer), sbuf,
//                    sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) == 0)
//        printf("host=%s, serv=%s\n", buffer, sbuf);
//
//    return "1";

}


const char *Clients::get_port() const{
    static char buffer[100];
    getnameinfo((struct sockaddr *)&address, address_length, 0, 0, buffer, sizeof(buffer), NI_NUMERICHOST);
    return buffer;


}

int Clients::get_rec_size() const {
    return rec_size;

}

void Clients::set_rec_size(int size) {
    rec_size = size;

}

void Clients::clear_requet()
{
    rec_size = 0;
    for(int i = 0; i < MAX_REQUEST_SIZE; i++)
        requet[i] = '\0';
}