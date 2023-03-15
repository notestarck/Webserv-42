//
// Created by Stephane Walter on 3/6/23.
//

#include "Master.hpp"


Master::Master(std::vector<Servers> servers){
//Master::Master(std::vector<Server> servers){
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
    std::cout << "wait test ok\n";
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
            std::cout << " Connection client : " << client.get_address()  << " " << client.get_port() << ".\n";
        }
    }
}

//void Master::recive() {
//    //ssize_t recv(int s, void *buf, ssize_t len, int flags);
//    //Ces fonctions renvoient le nombre d'octets reçus si elles réussissent, ou -1 si elles échouent. La valeur de retour sera 0 si le pair a effectué un arrêt normal.
//
//    for (unsigned long i = 0; i < clients.size(); i++) {
//        std::cout << clients.size() << std::endl;
//
//
//        if (FD_ISSET(clients[i].get_socket(), &reads)) {
//
//            if(MAX_REQUEST_SIZE == clients[i].get_rec_size())
//            {
//                //send_error(400, clients[i]);
//                //destroy client
//            }
//            int r = recv(clients[i].get_socket(), clients[i].requet + clients[i].get_rec_size(), MAX_REQUEST_SIZE - clients[i].get_rec_size(), 0);
//            clients[i].set_rec_size(clients[i].get_rec_size() + r);
//            if(clients[i].get_rec_size() > MAX_REQUEST_SIZE)
//            {
//                //send_error(413, clients[i]);
//                //destroy client
//            }
//            if(r < 0)
//            {
//                //send_error(500, clients[i]); // lol
//                i--;
//            }
//            if(r == 0)
//            {
//                i--;
//            }
//            else
//            {
//                Requet req = Requet(clients[i].get_socket());
//                int code;
//                if((code = req.parse(clients[i].requet)))
//                std::cout <<"parse requette" << std:: endl;
//                std::cout << code << "code retour" << std::endl;
//                {
//
//
//                }
//                std::string port = req.headers["Host"].substr(req.headers["Host"].find(':') + 1);
//                if(servers[req.headers["Host"]])
//                    clients[i].server = servers[req.headers["Host"]];
//                else
//                    // error 400
//                      // i--;
//                    continue;
//            }
//            clients[i].requet[clients.get_rec_size()] = 0;
//        }
//        Location* loc
//
//        if redir status
//        if !allow method
//        if  (loc & is_gci)
//        else{
//            if time out
//            if redir
//            if GET
//            if POST
//            if DELETE
//
//        clear requet;
//
//        }
//    }
//}

//void Master::send_error(int code, Clients &clients, method_type) {
//    std::iftream page;
//    if (clients.server->_error_page.find(code) != clients.server.end()) {
//        page.open(clients.server->_error_page[code]);
//
//        if (!page.is_open)
//            code = 404;
//    }
//
//    Reponse reponse(info_status[code]);
//

//    for (unsigned long i = 0; i < clients.size(); i++) {
//        std::cout << clients.size() << std::endl;
//
//
//        if (FD_ISSET(clients[i].get_socket(), &reads)) {
//            int s = clients[i].get_socket();
//            char *buf[4096];
//
//            std::string a = "HTTP/1.1 200 OK /r/n/r/nOK";
//            int i = send(s, &a, 4096, 0);
//            std::cout << "maeesage envoee" << std::endl;
//
//        }
//    }
//}




