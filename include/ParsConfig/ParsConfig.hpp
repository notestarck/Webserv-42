#ifndef _PARSCONFIG_HPP_
#define _PARSCONFIG_HPP_

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>

#define localhost "127.0.0.1"
using namespace std;

class ParsConfig
{
    public :
        class Location
        {
            public :
                Location(string allow, string root, string index);
                Location(const Location &srcs);
                ~Location();

                Location & operator=(const Location &srcs);
                const string & getAllow() const;
                const string & getRoot() const;
                const string & getIndex() const;

            private :
                Location();
                string _allow;
                string _root;
                string _index;
        };// class Locartion

        ParsConfig(ifstream &file_config, int indexServer);
        ParsConfig(const ParsConfig &srcs);
        ~ParsConfig();

        ParsConfig & operator=(const ParsConfig &srcs);

        //Getteur
        unsigned int getPort() const;

        //Setteur
        void    setIp(string ip);
        void    setPort(unsigned int port);
        void    setNameServer(string nameServer);
        void    setRoot(string root);
        void    setErrorPage(int error, string page);
        void    setLocation(string allow, string root, string index);

    private :
        ParsConfig();
        string              _ip;
        unsigned int        _port;
        string              _name_server;
        string              _root;
        map<int, string>    _error_page;
        vector<Location *>  _location;
};// class ParsConfig

#endif /* _PARSCONFIG_HPP_ */