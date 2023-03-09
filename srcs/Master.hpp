//
// Created by stephane walter on 04/03/2023.
//

#ifndef WEBSERV_MASTER_HPP
#define WEBSERV_MASTER_HPP

#include <unistd.h>
#include <sys/time.h>
#include <dirent.h>
#include <cstdio>
#include <sys/stat.h>
#include <sys/select.h>
#include <fstream>
#include <string.h>
#include <vector>
#include <map>
#include "Clients.hpp"
#include "Servers.hpp"

class Master {
private:
    std::vector<Servers> _servers;
    std::map<std::string, Servers *> servers;
    //std::map<std::string, Servers *> default_serv;

    std::vector<Clients> clients;

    int max_fd;

    std::map<int, std::string> info_status;    //gestions des status et erreurs

    Master();

public:

    fd_set	writes;
    fd_set	reads;

    Master(std::vector<Servers> servers);
    ~Master();

    void create_server();
    void wait();
    void add_fd_select(int fd, fd_set *fds);
    void run_select(fd_set *reads, fd_set *writes);
    void accept_socket();

    void recive();
    void sendr();
};








#endif //WEBSERV_MASTER_HPP
