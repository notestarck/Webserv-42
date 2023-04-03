/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reclaire <reclaire@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 13:50:45 by estarck           #+#    #+#             */
/*   Updated: 2023/03/29 10:56:55 by reclaire         ###   ########.fr       */
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


    _status_info.insert(std::pair<int, std::string>(200, "200 OK"));
    _status_info.insert(std::pair<int, std::string>(201, "201 Created"));
    _status_info.insert(std::pair<int, std::string>(204, "204 No Content"));
    _status_info.insert(std::pair<int, std::string>(404, "404 Not Found"));
    _status_info.insert(std::pair<int, std::string>(400, "400 Bad Request"));
    _status_info.insert(std::pair<int, std::string>(405, "405 Method Not Allowed"));
    _status_info.insert(std::pair<int, std::string>(408, "408 Request Timeout"));
    _status_info.insert(std::pair<int, std::string>(413, "413 Content Too Large"));
    _status_info.insert(std::pair<int, std::string>(414, "414 URI Too Long"));
    _status_info.insert(std::pair<int, std::string>(500, "500 Internal Server Error"));
    _status_info.insert(std::pair<int, std::string>(505, "505 HTTP Version Not Supported"));



}

Connection::Connection(const Connection & srcs)
{
	*this = srcs; 
}

Connection::~Connection()
{
	memset(&_read, 0, sizeof(_read));
	memset(&_write, 0, sizeof(_write));
	//memset(&_errors, 0, sizeof(_errors));
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
		//_errors = srcs._errors;
		_timeout = srcs._timeout;
        _status_info = srcs._status_info;
	}
	return (*this);
}

void Connection::initConnection()
{
	FD_ZERO(&_read);
	FD_ZERO(&_write);
	//FD_ZERO(&_errors);
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
	//int res = select(_maxFd + 1, &_read, &_write, &_errors, 0);
    int res = select(_maxFd + 1, &_read, &_write, 0, 0);
	if (res < 0)
	{
		std::cerr << "Error : Status of socket Connection::runSelect()" << std::endl;
		//exit (1);
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
            //std::cout << "la\n";
			Client newClient((*it)->getConfig());
			newClient._crecsize = sizeof(newClient._csin);
			if((*it)->hasCapacity())
			{
				int client_fd = accept((*it)->getSocket(), (sockaddr *)&newClient._csin, &newClient._crecsize);
				std::cout << "\033[0;32m client connecte \033[0m sur socket : " << newClient._csock << std::endl;
                if (client_fd < 0)
					std::cerr << "Failed to accept connection on port " << (*it)->getPort() << std::endl;
				else
				{
		 			(*it)->incrementCurrentConnection();
					newClient._config = (*it)->getConfig();
					newClient._location = (*it)->getLocation();
					newClient._csock = client_fd;
					_client.push_back(newClient);
                    //std::cout << "Get error 404 -----> " << _client.back()._config->getErrorPage(404) << std::endl;
					std::cout << "Accepted connection on port " << (*it)->getPort()  << std::endl;
				}
			}
			else
			{
				std::cerr << "Connection limit reached on port " << (*it)->getPort() << std::endl;
                // augmenter la capacite de connection
                //exit(1);
                //continue ;
                //boucle infini a gerer.
			}
		}
	}
}




// erase client si Connection=close
bool Connection::dead_or_alive(Client client, bool alive){
    if(alive)
        return false;
    //FD_ZERO(&_read);
    //FD_CLR(client._csock, &_read);
    close(client._csock);


    std::cout << "\033[0;31m client close \033[0m" << client._csock << std::endl;

    std::vector<Client>::iterator it;
    for(it = _client.begin(); it != _client.end(); it++ )
    {

        if((*it)._csock == client._csock)
        {
            std::cout << "client erase\n";
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
    //std::cout << "test live request\n";
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

	//Ces fonctions renvoient le nombre d'octets reçus si elles réussissent, ou -1 si elles échouent. La valeur de retour sera 0 si le pair a effectué un arrêt normal.
   for (std::vector<Client>::iterator it = _client.begin(); it < _client.end(); it++)
   {

    	if (FD_ISSET(it->_csock, &_read))
		{
            std::cout << it->_csock << " socket cloent\n";
            std::cout << " rec size = " << it->_recSize << std::endl;
        	if(MAX_REQUEST_SIZE == it->_recSize)
        	{
                send_error(400, *it, NULL);
                std::cerr << "Error : _recSize Connection::traitement() " << std::endl;
        		bool dead = dead_or_alive(*it, live_request(it->_recBuffer));
                if(dead)
                    it--;

				_client.erase(it);
                continue;
            }
            int ret = recv(it->_csock, it->_recBuffer, MAX_REQUEST_SIZE  , 0);

//              probleme sur recsize plus mai;
            if (it->_recSize > MAX_REQUEST_SIZE)
            {

                std::cerr << "Error : 413 Connection::traitement() _reSize" << std::endl;
                send_error(413, *it, NULL);
                bool dead = dead_or_alive(*it, live_request(it->_recBuffer));
                if(dead)
                    it--;
                //_client.erase(it);
                continue;


            }
            if (ret == 0)
			{
				std::cerr << "Error : Connection::traitement recv() reception" << std::endl;
                dead_or_alive(*it);
				it--;
			}
			else if (ret == SOCKET_ERROR)
			{
				std::cerr << "Error : 500 Connection::traitement recv() condition inattendue" << std::endl;
                send_error(500, *it, NULL);
                dead_or_alive(*it);
				it--;
			}

			it->_recSize += ret;   // pas forcement necessaire
            //else if
            if(request_ok(it->_recBuffer))
            {

             Request req = Request(it->_csock);
              int code;
              if((code = req.parse(it->_recBuffer)))
              {

                  send_error(code, *it, NULL);
                  bool dead = dead_or_alive(*it, live_request(&req.headers));
                  if(dead)
                      it--;
                  continue;
              }
                //std::cout << " req is ok\n";
                std::string port = req.headers["Host"].substr(req.headers["Host"].find(':') + 1);

//              if(it->_config->getHost() == "Host")
//              {
//                  it->_config.
//                  clients[i].server = servers[req.headers["Host"]];
//                    it->_config =
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
              if(req.headers.find("Content-Length") != req.headers.end() && stoi(req.headers["Content-Length"]) > 4096) //(it->_config->bodylimit_du_client
              {
                  send_error(413, *it, NULL);
                  bool dead = dead_or_alive(*it, live_request(&req.headers));
                  if(dead)
                      it--;
                  continue;
              }


//           it->req[clients.get_rec_size()] = 0;
//           it->_recBuffer->

//          check allow method dans location
//            Location getcur loc

//            method_ls = loc ? loc->allow method : it->_  ;

//            it->_config.getLocationAllow();
//
               std::vector<MethodType> method_ls;
              //method_ls = it->_location.;
                //method_ls.clear();
                method_ls.push_back(GET);
                method_ls.push_back(POST);
                method_ls.push_back(DELETE);


            if(!is_allowed_method(method_ls, req.method))
                {
                    std::cout << "check mothod non allowed\n";
                    send_error(405, *it, NULL);
                    bool dead = dead_or_alive(*it, live_request(&req.headers));
                    if(dead)
                        it--;
                    continue;
                }
            std::cout << " tqille locaion" << it->_location.size() << std::endl;
//            it->_location[1].
//
//         check cgi
//         if(it->_location.size() > 0 && 1)  //&& is_cgi
//         {
//             std::cout << "loc existe\n";
//         }

        //else
        {
//            if(rep_timeout(_client == true))
//            {
//                send_error(408, *it, NULL);
//
//            }


            if(!"redir")
            {
                std::cout << "REDIR\n";
                //                send_redir(it, req.method);
            }

            else if(req.method == "GET") {

                get_method(*it, req.path);
            }
            else if(req.method == "POST") {

                post_method(*it, req);
            }
            else if(req.method == "DELETE") {
                std::cout << "DELETE\n";
                delete_method(*it, req.path);
            }
        }
        bool dead = dead_or_alive(*it, live_request(&req.headers));
        if ( dead)
            it--;

        std::cout << "request completed\n";
        //close(it->_csock);
        //FD_CLR(it->_csock, &_read);
       // _client.erase(it);     //je tets





        it->_recSize = 0;
        for (int i = 0; i < MAX_REQUEST_SIZE; i++)
            it->_recBuffer[i]= '\0';
        FD_CLR(it->_csock, &_read);
        FD_ZERO(&_read);
        //close(it->_csock);
    }


}
}
   usleep(125);
}


int	send_check_err(Connection *connection, Client client, std::string str, size_t size)
{
	int send_ret_1 = send(client._csock, str.c_str(), size, 0);
	if (send_ret_1 < 0) {
		connection->send_error(500, client, NULL);
		return -1;
	} else if (send_ret_1 == 0) {
		connection->send_error(400, client, NULL);
		return -1;
	}
	return (0);
}


void Connection::get_method(Client &client, std::string path)
{
    std::cout << " GET method\n";
    std::cout << path << "path de la requete GET\n";
    if (path.length() >= MAX_URI_SIZE)
    {
        send_error(414, client, NULL);
        return;
    }
    struct stat buf;

    std::string full_path = find_path_in_root(path, client);
    std::cout << full_path << std::endl;
    if (stat(full_path.c_str(), &buf) < 0)
	{
		send_error(404, client, NULL);
		return;
	}

	if (S_ISDIR(buf.st_mode))
	{
        std::cout << "> Current path is directory\n";
        bool flag = false;
//		Location *loc = client.server->get_cur_location(path);
//		std::vector<std::string> indexes;
		std::string indexes;
//		if(client._location.size() > 0)
//		    indexes = client._location;
//		if (loc)
//		    indexes = loc->index;
//		else
		indexes = client._config->getIndex();
//		if (full_path.back() != '/')
//			full_path.append("/");
//		for (unsigned long i = 0; i < indexes.size(); i++)
//		{
//			FILE *tmp_fp = fopen((full_path + indexes[i]).c_str(), "rb");
//			if (tmp_fp)
//			{
//				fclose(tmp_fp);
//				full_path.append(indexes[i]);
//				flag = true;
//				break;
//			}
//		}
//		if (!flag)
//		{
//			if (client.server->autoindex)
//			{
//				send_autoindex_page(client, path);
//				return;
//			}
//			else
//			{
//				//send_error(404, client);
//				return;
//			}
//		}
    }

	FILE *fp = fopen(full_path.c_str(), "rb");
	fseek(fp, 0L, SEEK_END);
	size_t length = ftell(fp);
	rewind(fp);
	fclose(fp);


	//size_t length = buf.st_size;
    const char *type = find_type(full_path.c_str());



	Response response(_status_info[200]);
	response.append_header("Content-Length", longToString(length));
	response.append_header("Content-Type", type);


	std::string header = response.make_header();
	//int send_ret_1 = send(client._csock, header.c_str(), header.size(), 0);
	//if (send_ret_1 < 0) {
	//	send_error(500, client, NULL);
	//	return;
	//} else if (send_ret_1 == 0) {
	//	send_error(400, client, NULL);
	//	return;
	//}
	if (send_check_err(this, client, header, header.size()) < 0)
		return ;

	char buffer[BSIZE];
	int read_fd = open(full_path.c_str(), O_RDONLY);
	if (read_fd < 0) {
	    send_error(500, client, NULL);
	    return;
	}
	initSelect(read_fd, _read);
	runSelect();
	if (FD_ISSET(read_fd, &_read) == 0) {
	    send_error(400, client, NULL);
	    close(read_fd);
	    return;
	}
	int r = read(read_fd, buffer, BSIZE); //BSIZE
	if (r < 0)
	{
		send_error(500, client, NULL);
		close(read_fd);
		return;
	}

	while (r)
	{
	    //int send_ret_2 = send(client._csock, buffer, r, 0);
	    //if (send_ret_2 < 0)
	    //{
	    //    send_error(500, client, NULL);
	    //    break;
	    //}
	    //else if (send_ret_2 == 0)
	    //{
	    //    send_error(400, client, NULL);
	    //    break;
	    //}
		send_check_err(this, client, buffer, r);
	    initSelect(read_fd, _read);
	    runSelect();
	    if (FD_ISSET(read_fd, &_read) == 0)
	    {
	        send_error(400, client, NULL);
	        break;
	    }
	    r = read(read_fd, buffer, BSIZE);
	    if (r < 0)
	    {
	        send_error(500, client, NULL);
	        break;
	    }
	    if (r == 0)
	        break;
	    //close(client._csock); non marche pas
	}
    close(read_fd);
    //close(client._csock);



}

void Connection::post_method(Client &client, Request &request)
{
    std::cout << "POST method\n";

    if (request.headers["Transfer-Encoding"] != "chunked"
        && request.headers.find("Content-Length") == request.headers.end())
    {
        send_error(411, client, NULL);
        return;
    }

    std::string full_path = find_path_in_root(request.path, client);

    struct stat buf;
    lstat(full_path.c_str(), &buf);
    if (S_ISDIR(buf.st_mode))
    {
        if (request.headers.find("Content-Type") != request.headers.end())
        {
            size_t begin = request.headers["Content-Type"].find("boundary=");
            if (begin != std::string::npos)
            {
                std::string boundary = request.headers["Content-Type"].substr(begin + 9);
                begin = 0;
                size_t end = 0;
                std::string name;
                while (true)
                {
                    begin = request.body.find("name=", begin) + 6;
                    end = request.body.find_first_of("\"", begin);
                    if (begin == std::string::npos || end == std::string::npos)
                        break;
                    name = request.body.substr(begin, end - begin);
                    begin = request.body.find("\r\n\r\n", end) + 4;
                    end = request.body.find(boundary, begin);
                    if (begin == std::string::npos || end == std::string::npos)
                        break;
                    if (write_in_path(client, request.body.substr(begin, end - begin - 4), full_path + "/" + name) < 0)
                        break;
                    if (request.body[end + boundary.size()] == '-')
                        break;
                }
            }
            else
            {
                send_error(400, client, NULL);
                return;
            }
        }
        else
        {
            send_error(400, client, NULL);
            return;
        }
    }
    else
    {
        if (write_in_path(client, request.body, full_path) < 0)
            return;
    }

    int code = 201;
    if (request.headers["Content-Length"] == "0")
        code = 204;



    Response response(_status_info[code]);
    std::string header = response.make_header();

    int send_ret = send(client._csock, header.c_str(), header.size(), 0);

    if (send_ret < 0)
        send_error(500, client, NULL);
    else if (send_ret == 0)
        send_error(400, client, NULL);
    else
        std::cout << "> " << full_path << " posted(" << code << ")\n";
}


int	Connection::write_in_path(Client &client, std::string content, std::string path)
{
    std::cout << "> write in: " << path << "\n";
    size_t index = path.find_last_of("/");
    std::string file_name = path.substr(index + 1);
    std::string folder_path = path.substr(0, index);

    //https://koor.fr/C/cstdlib/system.wp

    std::string command = "mkdir -p " + folder_path;
    system(command.c_str());
    int write_fd = open(path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (write_fd < 0)
    {
        send_error(500, client, NULL);
        return -1;
    }

    initSelect(write_fd, _write);
    runSelect();
    if (FD_ISSET(write_fd, &_write) == 0)
    {
        send_error(500, client, NULL);
        close(write_fd);
        return -1;
    }
    int r = write(write_fd, content.c_str(), content.size());
    if (r < 0)
    {
        send_error(500, client, NULL);
        close(write_fd);
        return -1;
    }
    close(write_fd);
    return 0;
}
void Connection::delete_method(Client &client, std::string path){
    std::string full = find_path_in_root(path, client);

    FILE *fp = fopen(full.c_str(), "r");
    if(!fp)
    {
        send_error(404, client, NULL);
        return ;


    }
    fclose(fp);

    std::remove(full.c_str());
    Response res(_status_info[200]);

    std::string header = res.make_header();
    int send_re = send(client._csock, header.c_str(), header.size(),0);
    if(send_re < 0) {
        send_error(500, client, NULL);
    }
    else if(send_re == 0) {
        send_error (400, client, NULL);
    }
    std::cout << "delete ok\n";

}

void Connection::send_error(int code, Client &client, std::vector<MethodType> *allow_methods)
{

    std::cout << "> Send error page(" << code << ")" << " page erreur : " << client._config->getErrorPage(code) << std::endl;
    std::ifstream page;
    if(client._config->getErrorPage(code) != "notFound")
    {

        page.open(client._config->getErrorPage(code));
        if(!page.is_open())
            code = 404;

    }
    Response response(_status_info[code]);
    if (page.is_open())
    {

        std::string body;
        std::string line;
        while (page.good())
        {

            getline(page, line);
            body += line;
            body += '\n';
        }
        response.body = body;
        page.close();

    }
    else {

        response.make_status_body();
    }
    response.append_header("Content-Length", longToString(response.get_body_size()));
    response.append_header("Content-Type", "text/html");
    if (code == 405)
    {
//        std::string allowed_method_list;
//        for (unsigned long i = 0; i < (*allow_methods).size(); i++)
//        {
//            allowed_method_list += methodtype((*allow_methods)[i]);
//            if (i < (*allow_methods).size() - 1)
//                allowed_method_list += ", ";
//        }
        response.append_header("Allow", "GET");
    }

    std::string result = response.run_resp();
    int send_ret = send(client._csock, result.c_str(), result.size(), 0);
    if (send_ret < 0)
        std::cerr << "> Unexpected disconnect!\n";
    else if (send_ret == 0)
        std::cerr << "> The connection has been closed or 0 bytes were passed to send()!\n";
    //client.clear requete();
}


std::string Connection::find_path_in_root(std::string path, Client &client) const
{


    std::string full_path = "";
    //std::string location;
    full_path.append(client._config->getRoot());
    full_path.append("/");

    if(path == "/")
        full_path.append(client._config->getIndex());
    else {
        full_path.append(path);
        //full_path.append(client._config->getIndex());
    }
//    ou chercher dans location

//    client._location.
//    Location *loc = client._config->getIndex();
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

const char *Connection::find_type(const char *path) const{
    const char *point = strrchr(path, '.');
    if(point)
    {
        if(strcmp(point, ".css") == 0) return "text/css";
//        if(.csv)
        if(strcmp(point,".html") == 0) return "text/html";
//        if(.js)
//        if(.json)
        if(strcmp(point, ".pdf") == 0) return "application/pdf";
//        if(.gif)
        if(strcmp(point, ".jpeg") == 0) return "image/jpeg";
       if(strcmp(point,".mp4") == 0) return "video/mp4";
        if(strcmp(point, ".png") == 0) return "image/png";
        if(strcmp(point, ".ico") == 0) return "image/vnd.microsoft.icon";


    }
    return "text/plain";
}

std::string Connection::methodtype(MethodType method) const
{
    if (method == GET)
        return "GET";
    else if (method == POST)
        return "POST";
    else if (method == DELETE)
        return "DELETE";
    return "";
}

bool    Connection::is_allowed_method(std::vector<MethodType> allow_methods, std::string method)
{
    for (std::vector<MethodType>::iterator it = allow_methods.begin();
         it != allow_methods.end(); it++)
    {
        if (method == methodtype(*it))
            return true;
    }
    return false;
}