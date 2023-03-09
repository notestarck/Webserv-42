//
// Created by Stephane Walter on 3/6/23.
//

#include "Master.hpp"


Master::Master(std::vector<Servers> servers){
    _servers = servers;
    this->max_fd = -1;
}

Master::~Master(){}

void Master::create_server() {

        //std::cout << "debut creation serveur\n";

        std::vector<Servers>::iterator iter_begin = _servers.begin();
        std::vector<Servers>::iterator iter_end = _servers.end();
        while(iter_begin != iter_end)
        {
            servers.insert(std::pair<std::string, Servers *>("toto" , &*iter_begin));
            iter_begin++;
        }
        //std::cout << servers.at("toto") << "\n";

        std::map<std::string, Servers * >::iterator it;
        for(it = servers.begin(); it != servers.end(); it++) {
            (*it).second->create_socket();

            std::cout << " Create server " << (*it).first << "\n";

        }
}

void Master::wait() {

    //std::cout << "wait test debut\n";
    fd_set reads;
    fd_set writes;

    FD_ZERO(&reads);
    FD_ZERO(&writes);

    std::map<std::string, Servers *>::iterator it;
    for (it = servers.begin(); it != servers.end(); it++)
        add_fd_select((*it).second->listen_socket, &reads);

    for (unsigned long i = 0; i < clients.size(); i++)
        add_fd_select(clients[i].get_socket(), &reads);

    //http://manpagesfr.free.fr/man/man2/select.2.html
    run_select(&reads, &writes);
    //std::cout << "wait test ok\n";
    //gestion des fd des serveurs et clients

    // serveurs


}

// http://www.man-linux-magique.net/man2/FD_SET.html
void Master::add_fd_select(int fd, fd_set *fds){
    FD_SET(fd, fds);
    if(this->max_fd < fd)
    this->max_fd = fd;
    //std::cout << "max fd = " << this->max_fd << "\n";
}

void Master::run_select(fd_set *reads, fd_set *writes){
    //std::cout << "run selest debut\n";
    //std::cout << "max fd = " << this->max_fd << "\n";
    int res = 0;
    if((res = select((this->max_fd + 1), reads, writes , 0, 0 )) < 0)
    {
        exit(1);
    }
    if(res == 0)
        std::cout << "timeout select\n";
    this->reads = *reads;
    this->writes = *writes;
    //std::cout << "run selest fin\n";

}

void Master::accept_socket(){
    int server;

    std::map<std::string, Servers *>::iterator it;
    for(it = servers.begin(); it != servers.end(); it++)
    {
        server = (*it).second->listen_socket;
       if ((FD_ISSET(server, &reads)))
       {

            clients.push_back(Clients((*it).second));
            Clients &client = clients.back();

            client.set_socket(accept(server, (struct sockaddr*)&client.address, &client.address_length));
            if(client.get_socket() < 0) {
                std::cout << "error accept\n";
               exit(1);
            }
            std::cout << " Connection client : " << client.get_address() << ".\n";
        }
    }
}

void Master::recive() {
    //ssize_t recv(int s, void *buf, ssize_t len, int flags);
    //Ces fonctions renvoient le nombre d'octets reçus si elles réussissent, ou -1 si elles échouent. La valeur de retour sera 0 si le pair a effectué un arrêt normal.

    for (unsigned long i = 0; i < clients.size(); i++) {
        std::cout << clients.size() << std::endl;


        if (FD_ISSET(clients[i].get_socket(), &reads)) {
            int s = clients[i].get_socket();
            //std::cout << s << std::endl;
            // *buf donnees recues
            // len taille max
            // flags a 0
            char buf[4096];
            int r = recv(s, &buf, 4096, 0);
            std::cout << buf << std::endl;

        }
    }
}

void Master::sendr() {
    for (unsigned long i = 0; i < clients.size(); i++) {
        std::cout << clients.size() << std::endl;


        if (FD_ISSET(clients[i].get_socket(), &reads)) {
            int s = clients[i].get_socket();
            char *buf[4096];

            std::string a = "HTTP/1.1 200 OK";
            int i = send(s, &a, 4096, 0);
            std::cout << "maeesage envoee" << std::endl;

        }
    }
}




