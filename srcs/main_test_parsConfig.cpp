#include "../include/ParsConfig/ParsConfig.hpp"
#include "../include/Server.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

int countServer(ifstream &config_file)
{
    int     nbr_server = 0;
    char    *buf;

    while (!config_file.eof())
    {
        string  line;

        getline(config_file, line);
        if (line == "server {")
            nbr_server++;
    }
    return (nbr_server);
}

int main(int argc, char **argv)
{
    unsigned int    nbr_server;
    if (argc < 2)
    {
        cerr << "\033[1;31mError : Need < nginx_config_file >\033[0m" << endl;
        exit (-1);
    }
    const string  config_file_path(argv[1]);
    ifstream config_file(config_file_path.c_str());
    if (!config_file.is_open())
        cerr << "\033[1;31mError : Opening " << config_file_path << "\003[0m" << endl;
    nbr_server = countServer(config_file);
    config_file.seekg(0);

    vector<ParsConfig *>  my_config;
    for (unsigned int i = 0; i < nbr_server; i++)
    {
        ParsConfig  *tmp = new ParsConfig(config_file, i);
        my_config.push_back(tmp);
    }

    vector<ParsConfig *>::iterator it = my_config.begin();
    for (int i = 0; i + it < my_config.end(); i++)
    {
        cout << "\n---------- New Server -----------\n" << endl;
        cout << "\033[33mlisten : \033[0m" << my_config.at(i)->getPort() << endl;
        cout << "\033[33mip : \033[0m" << my_config.at(i)->getIp() << endl;
        cout << "\033[33mName server : \033[0m" << my_config.at(i)->getNameServer() << endl;
        cout << "\033[33mRoot : \033[0m" << my_config[i]->getRoot() << endl;
        cout << "\033[33mIndex : \033[0m" << my_config[i]->getIndex() << endl;
        cout << "\033[33mError : \033[0m" << my_config[i]->getErrorPage(500) << endl;
        cout << "\n\033[34m- Location -\033[0m" << endl;
        for(size_t j = 0; j < my_config[i]->getNbrLocation(); j++)
        {
            string  url = my_config[i]->getLocationUrl(j);
            cout << "\033[33mUrl : \033[0m" << url << endl;
            cout << "\t\033[33mAllow : \033[0m" << my_config[i]->getLocationAllow(url) << endl;
            cout << "\t\033[33mRoot : \033[0m" << my_config[i]->getLocationRoot(url) << endl;
            cout << "\t\033[33mIndex : \033[0m" << my_config[i]->getLocationIndex(url) << endl;
        }
        cout << endl;
    }

    for (int i = 0; i < nbr_server; i++)
        delete my_config[i];
    return 0;
}
