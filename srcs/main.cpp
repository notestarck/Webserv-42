#include "../include/ParsConfig/ParsConfig.hpp"
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
        cerr << "Error : Need < nginx_config_file >" << endl;
        exit (-1);
    }
    const string  config_file_path(argv[1]);
    ifstream config_file(config_file_path.c_str());
    if (!config_file.is_open())
        cerr << "Error : Opening " << config_file_path << endl;
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
        ParsConfig *tmp = my_config.at(i);
        cout << tmp->getPort() << endl;
    }

    // Creer vector<Server> mon_server(my_config);

    return 0;
}