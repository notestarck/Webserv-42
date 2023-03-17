/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 13:50:45 by estarck           #+#    #+#             */
/*   Updated: 2023/03/17 14:51:40 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/Connection.hpp"

Connection::Connection()
{}

Connection::Connection(std::vector<Server *> &servers) :
	_servers(servers),
	_maxFd(-1)
{
	_timeout.tv_sec = 1;
	_timeout.tv_usec = 0;
}

Connection::Connection(const Connection & srcs)
{
	*this = srcs; 
}

Connection::~Connection()
{
	memset(&_read, 0, sizeof(_read));
	memset(&_write, 0, sizeof(_write));
	memset(&_errors, 0, sizeof(_errors));
	for (std::vector<Client>::iterator it = _client.begin(); it < _client.end(); it++)
	{
		std::cout << "\033[32mLiberation des sockets clients\033[0m\n";
		close((*it)._csock);
	}
}

Connection & Connection::operator=(const Connection &srcs)
{
	if (this != &srcs)
	{
		_servers = srcs._servers;
		_client = srcs._client;
		_maxFd = srcs._maxFd;
		_read = srcs._read;
		_write = srcs._write;
		_errors = srcs._errors;
		_timeout = srcs._timeout;
	}
	return (*this);
}

void Connection::initConnection()
{
	FD_ZERO(&_read);
	FD_ZERO(&_write);
	FD_ZERO(&_errors);
	for (std::vector<Server *>::iterator it = _servers.begin(); it < _servers.end(); it++)
		initSelect((*it)->getSocket(), _read);
	for (std::vector<Client>::iterator it = _client.begin(); it < _client.end(); it++)
		initSelect(it->_csock, _read);
}

void Connection::initSelect(int fd, fd_set &set)
{
	FD_SET(fd, &set);
	if (fd > _maxFd)
		_maxFd = fd;
}

void Connection::runSelect()
{
	int res = select(_maxFd + 1, &_read, &_write, &_errors, 0);
	if (res < 0)
	{
		std::cerr << "Error : Status of socket Connection::runSelect()" << std::endl;
		exit (1);
	}
	else if (res == 0)
		std::cerr << "Error : Timeout Select Connection::runSelect()" << std::endl;
}

void Connection::acceptSocket()
{
	for (std::vector<Server *>::iterator it = _servers.begin(); it < _servers.end(); it++)
	{
		if (FD_ISSET((*it)->getSocket(), &_read))
		{
			//Socket server est pret a etre lu
			Client newClient;
			newClient._crecsize = sizeof(newClient._csin);
			if((*it)->hasCapacity())
			{
				int client_fd = accept((*it)->getSocket(), (sockaddr *)&newClient._csin, &newClient._crecsize);
				if (client_fd < 0)
					std::cerr << "Failed to accept connection on port " << (*it)->getPort() << std::endl;
				else
				{
					(*it)->incrementCurrentConnection();
					newClient._config = (*it)->getConfig();
					newClient._location = (*it)->getLocation();
					newClient._csock = client_fd;
					_client.push_back(newClient);
					std::cout << "Accepted connection on port " << (*it)->getPort() << std::endl;
				}
			}
			else
			{
				std::cerr << "Connection limit reached on port " << (*it)->getPort() << std::endl;
				//boucle infini a gerer.
			}
		}
	}
}

void Connection::send_error(int err)
{
	std::string html_page;

	//this->_servers
}

void Connection::traitement()
{
	//Ces fonctions renvoient le nombre d'octets reçus si elles réussissent, ou -1 si elles échouent. La valeur de retour sera 0 si le pair a effectué un arrêt normal.
   for (std::vector<Client>::iterator it = _client.begin(); it < _client.end(); it++)
	{
       std::cout << "Size de _client : " << _client.size() << std::endl;
    	if (FD_ISSET(it->_csock, &_read))
		{
			std::cout << "-------------gr ------------\n";
        	if(MAX_REQUEST_SIZE <= it->_recSize)
        	{
				std::cerr << "Error : _recSize Connection::traitement() " << std::endl;
        		//send_error(400, clients[i]);
				_client.erase(it);
          }
		   
          int ret = recv(it->_csock, it->_recBuffer, MAX_REQUEST_SIZE - 1, 0);
			if (ret == 0)
			{
				std::cerr << "Error : Connection::traitement recv() reception" << std::endl;
				it--;
			}
			else if (ret == SOCKET_ERROR)
			{
				std::cerr << "Error : 500 Connection::traitement recv() condition inattendue" << std::endl;
				it--;
			}

			it->_recSize += ret;
			if (it->_recSize > MAX_REQUEST_SIZE)
			{
				std::cerr << "Error : 413 Connection::traitement() _reSize" << std::endl;
				//send_error(413)...
				_client.erase(it);
			}
          else
          {
              Request req = Request(it->_csock);
              int code;
              if((code = req.parse(it->_recBuffer)))
              {
                  std::cout << " error code\n";
              }
                  std::cout << "code = " << code << std::endl;
              //std::cout <<"parse requette test" << std:: endl;
             // std::cout << code << "code retour" << std::endl;
             // {


              //}
              //std::string port = req.headers["Host"].substr(req.headers["Host"].find(':') + 1);
              //if(servers[req.headers["Host"]])
              //    clients[i].server = servers[req.headers["Host"]];
              //else
              //    // error 400
              //      // i--;
              //    break;;
          }
          //clients[i].requet[clients.get_rec_size()] = 0;
      }
	}
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
}