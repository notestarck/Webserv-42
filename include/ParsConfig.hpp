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
                Location(ifstream &file_config, string url);
                Location(const Location &srcs);
                ~Location();

                Location & operator=(const Location &srcs);
                const string & getUrl() const;
                const string & getAllow() const;
                const string & getRoot() const;
                const string & getIndex() const;

            private :
                Location();
                string _url;
                string _allow;
                string _root;
                string _index;
        };// class Locartion

        ParsConfig();
        ParsConfig(ifstream &file_config, int indexServer);
        ParsConfig(const ParsConfig &srcs);
        ~ParsConfig();

        ParsConfig & operator=(const ParsConfig &srcs);

        //Getteur
        string              getIp() const;
        unsigned int        getPort() const;
        string              getNameServer() const;
        string              getRoot() const;
        string              getIndex() const;
        string              getErrorPage(int code) const;
        string              getLocationUrl(size_t size) const;
        string              getLocationAllow(string url) const;
        string              getLocationRoot(string url) const;
        string              getLocationIndex(string url) const;
        size_t              getNbrLocation() const;

        //Setteur
        void    setIp(string ip);
        void    setPort(unsigned int port);
        void    setNameServer(string nameServer);
        void    setRoot(string root);
        void    setIndex(string index);
        void    setErrorPage(int error, string page);
        void    setLocation(ifstream &file_config, string url);

    private :
        string              _ip;
        unsigned int        _port;
        string              _name_server;
        string              _root;
        string              _index;
        map<int, string>    _error_page;
        vector<Location>    _location;
        size_t              _nbrLocation;
};// class ParsConfig

#endif /* _PARSCONFIG_HPP_ */