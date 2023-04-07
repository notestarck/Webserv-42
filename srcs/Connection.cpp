/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 13:50:45 by estarck           #+#    #+#             */
/*   Updated: 2023/04/07 17:49:25 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Connection.hpp"

#include <stdexcept>
#include <iostream>
#include <algorithm>

Connection::Connection()
{}

Connection::Connection(std::vector<Server*>& servers) :
	_servers(servers),
	_maxFd(-1)
{
	_timeout.tv_sec = 60;
	_timeout.tv_usec = 0;

	initMime();
}

Connection::Connection(const Connection& srcs)
{
	*this = srcs; 
}

Connection::~Connection()
{
	clearClientSockets();
}

Connection& Connection::operator=(const Connection& srcs)
{
	if (this != &srcs)
	{
		_servers = srcs._servers;
		_client = srcs._client;
		_maxFd = srcs._maxFd;
		_read = srcs._read;
		_write = srcs._write;
		_timeout = srcs._timeout;
		_mimeTypes = srcs._mimeTypes;
	}
	return (*this);
}

void Connection::initMime()
{
	_mimeTypes.insert(std::make_pair(".html", "text/html"));
	_mimeTypes.insert(std::make_pair(".css", "text/css"));
	_mimeTypes.insert(std::make_pair(".js", "application/javascript"));
	_mimeTypes.insert(std::make_pair(".pdf", "application/pdf"));
	_mimeTypes.insert(std::make_pair(".jpg", "image/jpeg"));
	_mimeTypes.insert(std::make_pair(".png", "image/png"));
	_mimeTypes.insert(std::make_pair(".gif", "image/gif"));
	_mimeTypes.insert(std::make_pair(".mp4", "video/mp4"));
	_mimeTypes.insert(std::make_pair(".ico", "image/vnd.microsoft.icon"));
	std::cout << "MIME types initialized. Map size: " << _mimeTypes.size() << std::endl;
}

void Connection::initConnection()
{
	FD_ZERO(&_read);
	FD_ZERO(&_write);
	for (std::vector<Server *>::iterator it = _servers.begin(); it < _servers.end(); it++)
		initSelect((*it)->getSocket(), _read);
	for (std::vector<Client>::iterator it = _client.begin(); it < _client.end(); it++)
		initSelect(it->_csock, _read);
}

void Connection::initSelect(int fd, fd_set& set)
{
	FD_SET(fd, &set);
	if (fd > _maxFd)
		_maxFd = fd;
}

void Connection::runSelect()
{
    int res = select(_maxFd + 1, &_read, &_write, 0, 0);
    if (res < 0)
    {
        if (errno == EINTR) // Vérifier si l'erreur est due à un signal
            return; // Retourner à la boucle principale
        else
            throw std::runtime_error("select failed");
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
            Client newClient((*it)->getConfig(), (*it)->getServer(), (*it)->getLocation());
            newClient._crecSize = sizeof(newClient._csin);

            if ((*it)->hasCapacity())
            {
                newClient._csock = accept((*it)->getSocket(), (sockaddr*)&newClient._csin, &newClient._crecSize);
                if (newClient._csock < 0)
                {
                    std::cerr << "Failed to accept connection on port " << (*it)->getPort() << ", error code: " << errno << ", error message: " << strerror(errno) << std::endl;
					close(newClient._csock);
                    continue;
                }
                std::cout << "\033[0;32mAccepted connection \033[0m on port " << (*it)->getPort() << std::endl;
                std::cout << "\033[0;32mClient connected \033[0m on socket: " << newClient._csock << std::endl;

                (*it)->incrementCurrentConnection();

                _client.push_back(newClient);
            }
        }
    }
}

void Connection::clearClientSockets()
{
	//memset(&_read, 0, sizeof(_read));
	//memset(&_write, 0, sizeof(_write));
	//memset(&_errors, 0, sizeof(_errors));
	for (std::vector<Client>::iterator it = _client.begin(); it < _client.end(); it++)
	{
		std::cout << "\033[32mLiberation des sockets clients\033[0m\n";
		if (close((*it)._csock) != 0)
			std::cerr << "Error while closing socket: " << strerror(errno) << std::endl;
	}
	_client.clear();
}

// erase client si Connection=close
bool Connection::deadOrAlive(Client client, bool alive)
{
    if (alive)
        return false;

    client._server.decrementCurrentConnection();
	int tmpSock = client._csock;
    FD_CLR(client._csock, &_read);
    shutdown(client._csock, SHUT_RDWR);
    if (close(client._csock) != 0)
        std::cerr << "Error while closing socket: " << strerror(errno) << std::endl;

    std::cout << "\033[0;31m client close \033[0m" << client._csock << std::endl;

    std::vector<Client>::iterator it;
    for(it = _client.begin(); it != _client.end(); it++ )
    {
        if((*it)._csock == tmpSock)
        {
            std::cout << "client erase\n";
            _client.erase(it);
            break;
        }
    }
    return true;
}

// connection vivante : https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Connection
bool	Connection::liveRequest(char *request) const
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
bool Connection::liveRequest(std::map<std::string, std::string> *headers) const
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

bool    Connection::checkRequest(char *request)
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

void Connection::traitement()
{
	for (std::vector<Client>::iterator it = _client.begin(); it < _client.end(); it++)
	{
		if (FD_ISSET(it->_csock, &_read))
		{
			if (!readClientRequest(*it))
			{
				deadOrAlive(*it);
				continue;
			}
			handleRequest(*it);
				deadOrAlive(*it);
		}
	}
}

bool Connection::readClientRequest(Client &client)
{
    int bytesReceived = recv(client._csock, client._recBuffer, sizeof(client._recBuffer), 0);

    if (client._crecSize > MAX_REQUEST_SIZE || client._crecSize == MAX_REQUEST_SIZE)
    {
        std::cerr << "Error : 413 Request size exceeds the limit (" << MAX_REQUEST_SIZE << " bytes) for client: " << client._csock << std::endl;
		sendErrorResponse(client, 413);
        return (false);
    }

    if (bytesReceived < 0)
    {
        std::cerr << "Error : 500 receiving data from client: " << client._csock << std::endl;
		sendErrorResponse(client, 500);
        return (false);
    }
    else if (bytesReceived == 0)
    {
        std::cout << "Client disconnected: " << client._csock << std::endl;
        return (false);
    }

	if (checkRequest(client._recBuffer))
    {
        client._recBuffer[bytesReceived] = '\0';
        std::string requestStr(client._recBuffer);
        std::cout << std::endl << "\033[0;32mReceived request from client \033[0m"
								<< client._csock << ": " << std::endl
								<< requestStr
								<< "\033[0;32mEnd of request client \033[0m" << std::endl;
		HTTPRequest httpRequest(requestStr, client);
		return (true);
    }
	std::cerr << "Error : Connection::readClientRequest checkRequest(), request non conform" << std::endl;
	return (false);
}

void Connection::handleRequest(Client &client)
{
	switch (client._method)
	{
        case GET:
            handleGET(client);
            break;
        case POST:
            handlePOST(client);
            break;
        case DELETE:
            handleDELETE(client);
            break;
        default:
            // Gérer les méthodes inconnues
            break;
    }
}

void Connection::handleGET(Client& client)
{
   if (client._uri.length() >= MAX_URI_SIZE)
   {
        std::cerr << "Error : 414 URI Too Long from client: " << client._csock << std::endl;
      sendErrorResponse(client, 414);
        return;
   }
    std::string filePath = getFilePath(client);

    int file_fd = open(filePath.c_str(), O_RDONLY);
    if(file_fd == -1){
        //erreur ouvertue fichier"
        close(client._csock);

    }
    off_t file_size  = lseek(file_fd, 0, SEEK_END);
    lseek(file_fd, 0, SEEK_SET);

    //reponse
    //sendHttpResponse(client, 200, getMimeType(filePath), body);
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: " << getMimeType(filePath) <<"\r\n";
    response << "Content-Length: " << file_size << "\r\n\r\n";

    //envoyer la reponse

    std::string response_str = response.str();
    int bytes_sent = send(client._csock, response_str.c_str(), response_str.size(), 0);
    if (bytes_sent == -1) {
        //"Erreur lors de l'envoi de la réponse HTTP au client"
        close(file_fd);
        close(client._csock);

    }

    //envoie du fichier octet par octet
    char file_buffer[1024];
    while (true) {
        int bytes_read = read(file_fd, file_buffer, sizeof(file_buffer));
        if (bytes_read == -1) {
            //Erreur lors de la lecture du fichier
            break;

        }

        if (bytes_read == 0) {
            // Fin du fichier atteinte
            break;
        }

        bytes_sent = send(client._csock, file_buffer, bytes_read, 0);
        if (bytes_sent == -1) {
            //"Erreur lors de l'envoi du fichier au client"
            break;
        }
    }
        close(file_fd);
    //std::ifstream file(filePath, std::ios::in | std::ios::binary);
//    if (file.is_open())
// {
//        std::string body((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//        file.close();
//        sendHttpResponse(client, 200, getMimeType(filePath), body);
//    }
// else
// {
//        std::cerr << "Error : 404 Not Found from client: " << client._csock << std::endl;
//        sendErrorResponse(client, 404);
// }
}

void Connection::handlePOST(Client& client)
{
    // Vérification si l'URI correspond à une configuration de location
    ParsConfig::Location *location = findLocationForUri(client._uri, client._location);
    if (!location || !location->isMethodAllowed("POST"))
	{
        std::cerr << "Error : 405 Method Not Allowed from client: " << client._csock << std::endl;
        sendErrorResponse(client, 405);
        return;
    }

	// Si pas de cgiPath, on upload les donnees
	if (location->getCgiPath().empty())
	{
        std::string uploadPath = location->getRoot();
        if (uploadPath.empty())
        {
            std::cerr << "Error : 403 Forbidden from client: " << client._csock << std::endl;
            sendErrorResponse(client, 403);
            return;
        }
	
		// Extraire le nom du fichier à partir de l'URI
        std::string fileName = client._uri.substr(client._uri.find_last_of('/') + 1);

        // Créer le chemin complet vers le fichier à enregistrer
        std::string filePath = uploadPath + "/" + fileName;
		std::cout << "-----------------------> filePath" << filePath << std::endl;

        // Stocker les données reçues
        std::ofstream outFile(filePath, std::ios_base::binary);
        if (outFile.is_open())
        {
            outFile.write(client._recBuffer, client._crecSize);
            outFile.close();
            sendHttpResponse(client, 201, "Created", "Fichier enregistré avec succès.");
        }
        else
        {
            std::cerr << "Error : 500 Internal Server Error from client: " << client._csock << std::endl;
            sendErrorResponse(client, 500);
        }
    }
    else
    {
        // Lancer le cgiPath avec les données reçues
        executeCGI(client, location->getCgiPath());
    }
}

void Connection::handleDELETE(Client& client)
{
	(void)client;
    // Vérifiez si la ressource existe et les conditions sont remplies
    // Générez la réponse HTTP appropriée
    // Envoyez la réponse au client
}

std::string Connection::getFilePath(const Client &client)
{
    std::string basePath = client._config.getRoot();
    std::string filePath = basePath + client._uri;

    if (filePath.back() == '/')
        filePath += client._config.getIndex();

    return filePath;
}

std::string Connection::getMimeType(const std::string& filePath)
{
   std::string fileExtension = filePath.substr(filePath.find_last_of('.'));
   if (_mimeTypes.find(fileExtension) != _mimeTypes.end())
       return _mimeTypes[fileExtension];
	else
       return "application/octet-stream"; // Type MIME par défaut ?? ou "text/plain"
}

ParsConfig::Location *Connection::findLocationForUri(const std::string& uri, const std::vector<ParsConfig::Location>& locations)
{
    for (size_t i = 0; i < locations.size(); i++)
	{
        if (uri.find(locations[i].getUrl()) == 0)
            return (const_cast<ParsConfig::Location*>(&locations[i]));
    }
    return (nullptr);
}

void Connection::executeCGI(Client &client, const std::string &cgiPath)
{
    int cgiInput[2];  // Pipe envoyer les données POST au script CGI
    int cgiOutput[2]; // Pipe pour lire la sortie du script CGI

    if (pipe(cgiInput) < 0 || pipe(cgiOutput) < 0)
	{
        sendErrorResponse(client, 500);
        return;
    }

    pid_t pid = fork();
    if (pid < 0)
	{
        sendErrorResponse(client, 500);
        return;
    }
    if (pid == 0)
	{
        close(cgiInput[1]);
        close(cgiOutput[0]);

        dup2(cgiInput[0], STDIN_FILENO);
        dup2(cgiOutput[1], STDOUT_FILENO);

        char *argv[] = {const_cast<char *>(cgiPath.c_str()), NULL};
		char *envp[] = {NULL};
		if (execve(cgiPath.c_str(), argv, envp) == -1)
		{
        	sendErrorResponse(client, 500);
        	exit(EXIT_FAILURE);
		}
    }
	else
	{
        close(cgiInput[0]);
        close(cgiOutput[1]);

        write(cgiInput[1], client._requestStr.c_str(), client._requestStr.length());
        close(cgiInput[1]);

        // Lire la sortie du script CGI depuis le pipe de sortie
        char buffer[4096];
        std::string cgiResponse;
        ssize_t bytesRead;
        while ((bytesRead = read(cgiOutput[0], buffer, sizeof(buffer) - 1)) > 0)
		{
            buffer[bytesRead] = '\0';
            cgiResponse += buffer;
        }
        close(cgiOutput[0]);

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		{
            // Envoyer la réponse CGI au client
            send(client._csock, cgiResponse.c_str(), cgiResponse.length(), 0);
        }
		else
		{
			sendErrorResponse(client, 500);
    	}
	}
}