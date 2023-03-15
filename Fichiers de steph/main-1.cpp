//
// Created by stephane walter on 04/03/2023.
//

#include "Master.hpp"
#include "Servers.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    if(argc > 2)
        return 0;
    std::cout << "**************************\n";
    std::cout << "*            WEBSERV     *\n";
    std::cout << "**************************\n";

//    std::string config;
//    if(argc == 1)
//       std::cout << "config default\n";// config par default
//
//    else
//        std::cout << "config .conf\n";

    // fichier de config
    std::vector<Servers> *servers;

    servers = new std::vector<Servers>;
    servers->push_back(Servers("127.0.0.1", "80"));
    std::cout << servers->at(0).getHost() << "\n";


    Master master(*servers);


    master.create_server(); // creation des serveurs  puis create_socket

    //std::cout << "test create serv\n";
    while(1) {
        master.wait();


        //master.accept_socket();
        //master.recive();
        //master.sendr();

        //taitement requetes
    }

    //master.close_servers;
    delete servers;
    return 0;
}
