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
    test = 42;

   std::cout << "Consyructor ok\n" ;
   _status_info.insert(std::pair<int, std::string>(200, "200 OK"));
   std::cout << _status_info[200] << std::endl;
    //_status_info.insert(std::make_pair(201, " 201 pas ok"));
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
        _status_info = srcs._status_info;
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

//void Connection::send_error(int err)
//{
//	std::string html_page;
//
//	//this->_servers
//}


// erase client si Connection=close
bool Connection::dead_or_alive(Client client, bool alive){
    if(alive)
        return false;
    close(client._csock);

    std::vector<Client>::iterator it;
    for(it = _client.begin(); it != _client.end(); it++ )
    {
        if((*it)._csock == client._csock)
        {
            _client.erase(it);
            return true;
        }
    }
    exit(1);
}
// connection vivante : https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Connection
bool	Connection::live_request(char *request) const
{
    char *body = strstr(request, "\r\n\r\n");
    if (!body)
        return false;
    body += 4;

    char *connection;
    if ((connection = strnstr(request, "Connection", strlen(request) - strlen(body))))
    {
        if (strnstr(connection, "close", strlen(request) - strlen(body)))
            return false;
        return true;
    }
    return true;
}

// connection vivante : https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Connection
bool	Connection::live_request(std::map<std::string, std::string> *headers) const
{
    if (headers->find("Connection") != headers->end())
    {
        if ((*headers)["Connection"] == "close")
            return false;
        return true;
    }
    return true;
}

//verification des erreurs de la requete
bool    Connection::request_ok(char *request)
{
    char *body = strstr(request, "\r\n\r\n");
    if (!body)
        return false;
    body += 4;
    if (strnstr(request, "chunked", strlen(request) - strlen(body)))
    {
        if (strstr(body, "\r\n\r\n"))
            return true;
        return false;
    }

    // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Length
    else if (strnstr(request, "Content-Length", strlen(request) - strlen(body)))
    {
        if (strstr(body, "\r\n\r\n"))
            return true;
        char *start = strnstr(request, "Content-Length: ", strlen(request) - strlen(body)) + 16;
        char *end = strstr(start, "\r\n");
        char *len = strndup(start, end - start);
        free(len);
        int len_i = atoi(len);
        if ((size_t)len_i <= strlen(body))
            return true;
        return false;
    }
    else if (strnstr(request, "boundary=", strlen(request) - strlen(body)))
    {
        if (strstr(body, "\r\n\r\n"))
            return true;
        return false;
    }
    return true;
}

//bool Connection::rep_timeout(Client& client)
//{
//    static timeval tv;
//
//    gettimeofday(&tv, NULL);
//    if (tv.tv_sec - client._last_time.tv_sec > client.server->recv_timeout.tv_sec)
//        return true;
//    client.set_last_time_sec(tv);
//    return false;
//}
void Connection::traitement()
{
    std::cout << " test deb = " << test << std::endl;
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
        		bool dead = dead_or_alive(*it, live_request(it->_recBuffer));
                if(dead)
                    it--;
                //send_error(400, *it);
				//_client.erase(it);
                continue;
            }
            int ret = recv(it->_csock, it->_recBuffer, MAX_REQUEST_SIZE - 1, 0);
			if (ret == 0)
			{
				std::cerr << "Error : Connection::traitement recv() reception" << std::endl;
                dead_or_alive(*it);
				it--;
			}
			else if (ret == SOCKET_ERROR)
			{
				std::cerr << "Error : 500 Connection::traitement recv() condition inattendue" << std::endl;
                // send_error(500, *it);
                dead_or_alive(*it);
				it--;
			}

			it->_recSize += ret;
			if (it->_recSize > MAX_REQUEST_SIZE)
			{
				std::cerr << "Error : 413 Connection::traitement() _reSize" << std::endl;

                bool dead = dead_or_alive(*it, live_request(it->_recBuffer));
                if(dead)
                    it--;

                continue;
                //send_error(413)...
				//_client.erase(it);
			}
            else if(request_ok(it->_recBuffer))
            {

             Request req = Request(it->_csock);
              int code;
              if((code = req.parse(it->_recBuffer)))
              {
                  std::cout << " error code\n";
                  // semnd_erro
                  bool dead = dead_or_alive(*it, live_request(&req.headers));
                  if(dead)
                      it--;
                  continue;
              }
                std::cout << " req is ok\n";
              std::string port = req.headers["Host"].substr(req.headers["Host"].find(':') + 1);

//              if(req.headers["Host"])
//              {
//                 // clients[i].server = servers[req.headers["Host"]];
//              }
//              else

//              {
//                  //send_error(400, it);
//                  bool dead = dead_or_alive(*it, live_request(&req.headers));
//                  if(dead)
//                      it--;
//                  continue;
//
//              }
              if(req.headers.find("Content-Length") != req.headers.end() && stoi(req.headers["Content-Length"]) > 4096) //(it->_config.bodylimit_du_client
              {
                  //send_error(413, it);
                  bool dead = dead_or_alive(*it, live_request(&req.headers));
                  if(dead)
                      it--;
                  continue;
              }


//           it->req[clients.get_rec_size()] = 0;
//           it->_recBuffer->

       // check allow method dans location

//         check cgi
//         if(it->_location.size() > 0 && 1)  //&& is_cgi
//         {
//             std::cout << "loc existe\n";
//         }

        else
        {
//            if(rep_timeout(_client == true))
//            {
//                send_error(408, it);
//
//            }
            std::cout << req.method << std::endl;

            if(!"redir")
            {
                std::cout << "REDIR\n";
                //                send_redir(it, req.method);
            }
            else if(req.method == "GET") {
                std::cout << "GET method demandeee\n";
                get_method(*it, req.path);
            }
            else if(req.method == "POST")
                std::cout << "POST\n";
               // post_method(it, req);
            else if(req.method == "DELETE")
                std::cout << "DELETE\n";
                //delete_method(it, req.path);
        }
        bool dead = dead_or_alive(*it, live_request(&req.headers));
        if ( dead)
            it--;
        std::cout << "request completed\n";
        //it->clear_request;
    }


}
}
   usleep(200);
}

void Connection::get_method(Client &client, std::string path)
{
    std::cout << " test0 = " << test << std::endl;
    if (path.length() >= MAX_URI_SIZE)
    {
        //send_error_page(414, client);
        return;
    }
    struct stat buf;
    std::cout << "index" << client._config.getIndex() << std::endl;
    std::string full_path = find_path_in_root(path, client);
    std::cout << full_path << std::endl;
    lstat(full_path.c_str(), &buf);
    FILE *check_fp = fopen(full_path.c_str(), "rb");

    if (!check_fp)
        std::cout << "error page 404\n";
        //send_error_page(404, client);
    else
    {
        fclose(check_fp);
        if (S_ISDIR(buf.st_mode)) {
            std::cout << "> Current path is directory\n";
            bool flag = false;
//            Location *loc = client.server->get_cur_location(path);
//            std::vector<std::string> indexes;
            std::string indexes;
//            if(client._location.size() > 0)
//                indexes = client._location;
//            if (loc)
//                indexes = loc->index;
//            else
            indexes = client._config.getIndex();
            if (full_path.back() != '/')
                full_path.append("/");
//            for (unsigned long i = 0; i < indexes.size(); i++)
//            {
//                FILE *tmp_fp = fopen((full_path + indexes[i]).c_str(), "rb");
//                if (tmp_fp)
//                {
//                    fclose(tmp_fp);
//                    full_path.append(indexes[i]);
//                    flag = true;
//                    break;
//                }
//            }
//            if (!flag)
//            {
//                if (client.server->autoindex)
//                {
//                    send_autoindex_page(client, path);
//                    return;
//                }
//                else
//                {
//                    //send_error(404, client);
//                    return;
//                }
//           }
        }
            FILE *fp = fopen(full_path.c_str(), "rb");
            fseek(fp, 0L, SEEK_END);
            size_t length = ftell(fp);
            rewind(fp);
            fclose(fp);
//        const char *type = find_content_type(full_path.c_str());
            std::cout << " ici tout va bien\n";
            std::cout << length << "lemghft \n";
            std::cout << " test = " << test << std::endl;
            std::cout <<"status info" << _status_info[200]<< "size" << _status_info.size() << std::endl;
           // Response response("200 OK");
            Response response(_status_info[200]);
            std::cout << " ici tout va bien2\n";
            response.append_header("Content-Length", longToString(length));
            std::cout << " ici tout va bien3\n";
//        response.append_header("Content-Type", type);
            response.append_header("Content-Type", "text");
//
            std::string header = response.make_header();
            int send_ret_1 = send(client._csock, header.c_str(), header.size(), 0);
            if (send_ret_1 < 0) {
                std::cout << "error 500\n";
                // send_error(500, client, NULL);
                return;
            } else if (send_ret_1 == 0) {
                std::cout << "error 400\n";
//             send_error(400, client, NULL);
                return;
            }
//
            char buffer[BSIZE];
            int read_fd = open(full_path.c_str(), O_RDONLY);
            if (read_fd < 0) {
                //send_error(500, client);
                return;
            }
            initSelect(read_fd, _read);
            runSelect();
            if (FD_ISSET(read_fd, &_read) == 0) {
//            send_error_page(400, client);
                close(read_fd);
                return;
            }
            int r = read(read_fd, buffer, BSIZE);
            if (r < 0)
            {}
//            send_error_page(500, client);

            else

            {
            int send_ret_2;
            while (r)
            {
                send_ret_2 = send(client._csock, buffer, r, 0);
                if (send_ret_2 < 0)
                {
//                    send_error_page(500, client);
                    break;
                }
                else if (send_ret_2 == 0)
                {
//                    send_error_page(400, client);
                    break;
                }
                initSelect(read_fd, _read);
                runSelect();
                if (FD_ISSET(read_fd, &_read) == 0)
                {
//                    //send_error(400, client);
                    break;
                }
                r = read(read_fd, buffer, BSIZE);
                if (r < 0)
                {
//                    send_error_page(500, client);
                    break;
                }
                if (r == 0)
                    break;
            }
        }
        close(read_fd);
    }
}


//void Connection::send_error(int code, Client &client, std::vector<MethodType> *allow_methods)
//{
//    std::cout << "> Send error page(" << code << ")\n";
//    std::ifstream page;
//
//    if (client.server->error_pages.find(code) != client.server->error_pages.end())
//    {
//        page.open(client.server->error_pages[code]);
//        if (!page.is_open())
//            code = 404;
//    }
//
//    Response response(status_info[code]);
//    if (page.is_open())
//    {
//        std::string body;
//        std::string line;
//        while (!page.eof())
//        {
//            getline(page, line);
//            body += line;
//            body += '\n';
//        }
//        response.body = body;
//        page.close();
//    }
//    else
//        response.make_status_body();
//
//    response.append_header("Content-Length", number_to_string(response.get_body_size()));
//    response.append_header("Content-Type", "text/html");
//    if (code == 405)
//    {
//        std::string allowed_method_list;
//        for (unsigned long i = 0; i < (*allow_methods).size(); i++)
//        {
//            allowed_method_list += methodtype_to_s((*allow_methods)[i]);
//            if (i < (*allow_methods).size() - 1)
//                allowed_method_list += ", ";
//        }
//        response.append_header("Allow", allowed_method_list);
//    }
//
//    std::string result = response.serialize();
//    int send_ret = send(client.get_socket(), result.c_str(), result.size(), 0);
//    if (send_ret < 0)
//        std::cerr << "> Unexpected disconnect!\n";
//    else if (send_ret == 0)
//        std::cerr << "> The connection has been closed or 0 bytes were passed to send()!\n";
//    client.clear_request();
//}


std::string Connection::find_path_in_root(std::string path, Client &client) const
{
    std::string full_path = "";
//    std::string location;
    //full_path.append(client.get_root_path(path));
    full_path.append(client._config.getRoot());
    full_path.append("/");
    std::cout << client._config.getIndex() << std::endl;
    full_path.append(client._config.getIndex());
//    client._location.
//    Location *loc = client.server->get_cur_location(path);
//    if (loc)
//        location = loc->path;
//    else
//        location = "";
//    std::string str = path.substr(location.length());
//    full_path.append(str);
    return full_path;
}

std::string Connection::longToString(long number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}