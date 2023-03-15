//
// Created by Stephane Walter on 3/7/23.
//

#include "Servers.hpp"

Servers::Servers() {}

Servers::~Servers() {}

Servers::Servers(std::string host, std::string port) {


    this->host = host;//inet_addr(host);
    this->port = port; //htons(port);
}



void Servers::create_socket()
{
    //std::cout << "creation socket debut\n";
    // http://manpagesfr.free.fr/man/man3/getaddrinfo.3.html
    struct addrinfo  hints;
    struct addrinfo  *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;


    sockaddr_in toto;
    memset(&toto, 0, sizeof(toto));
    toto.sin_family = AF_INET;
    toto.sin_addr.s_addr = INADDR_ANY;
    toto.sin_port = htons(80);

    //int getaddrinfo(const char *node, const char *service,const struct addrinfo *hints, struct addrinfo **res);

    if(getaddrinfo(host.c_str(), port.c_str(), &hints, &res ) >= 0)

    {
        //std::cout << "ok create socket\n";
        int new_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        //http://manpagesfr.free.fr/man/man2/fcntl.2.html
        fcntl(new_socket, F_SETFL, O_NONBLOCK);
        int optval = 1;
        if(new_socket < 0)
        {
            std::cout << "error create socket\n";
            exit (1);
        }

        //https://manpages.ubuntu.com/manpages/bionic/fr/man2/setsockopt.2.html

        if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof(optval)) < 0 )
        {

            std::cout << " stesockopt error \n";
        }
        if(bind(new_socket, (sockaddr *)&toto  , sizeof(toto) ) != 0)
        {
            std::cout << "bind errorn\n";
            exit(1);

        }
        freeaddrinfo(res);

        //http://manpagesfr.free.fr/man/man2/listen.2.html
        if(listen(new_socket, 10) < 0)
        {
            std::cout << " error listen\n";
            exit(1);
        }
        this->listen_socket = new_socket;
    }
    else
        std::cout << "error create getaddrinfo\n";
    //exit(1);

    //std::cout << "creation socket ok\n";
}

std::string Servers::getHost() {
    return this->host;
}