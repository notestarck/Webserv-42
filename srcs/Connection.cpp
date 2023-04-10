/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 13:50:45 by estarck           #+#    #+#             */
/*   Updated: 2023/04/10 10:24:52 by estarck          ###   ########.fr       */
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
	_mimeTypes.insert(std::make_pair(".odt", "application/vnd.oasis.opendocument.text"));
	_mimeTypes.insert(std::make_pair(".rtf", "application/rtf"));
	_mimeTypes.insert(std::make_pair(".jpg", "image/jpeg"));
	_mimeTypes.insert(std::make_pair(".png", "image/png"));
	_mimeTypes.insert(std::make_pair(".gif", "image/gif"));
	_mimeTypes.insert(std::make_pair(".mp4", "video/mp4"));
	_mimeTypes.insert(std::make_pair(".ico", "image/vnd.microsoft.icon"));
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
    int res = select(_maxFd + 1, &_read, &_write, 0, &_timeout);
    if (res < 0)
    {
        if (errno == EINTR) // Vérifier si l'erreur est due à un signal
            return;
        else
            throw std::runtime_error("select failed");
    }
	else if (res == 0)
		std::cerr << "Client: wait Connection::runSelect()" << std::endl;
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
        client._requestStr = client._recBuffer;
        std::cout << std::endl << "\033[0;32mReceived request from client \033[0m"
								<< client._csock << ": " << std::endl
								<< client._requestStr
								<< "\033[0;32mEnd of request client \033[0m" << std::endl;
		HTTPRequest httpRequest(client._requestStr, client);
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
    //ios::binary pour indiquer que le fichier doit être traité en mode binaire plutôt qu'en mode texte. 
    std::ifstream file(filePath, std::ios::in | std::ios::binary);

    if (file.is_open())
	{
        std::string body((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        sendHttpResponse(client, 200, getMimeType(filePath), body);
    }
	else
	{
        std::cerr << "Error : 404 Not Found from client: " << client._csock << std::endl;
        sendErrorResponse(client, 404);
	}
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

    // Si pas de cgiPath, on upload les données
    if (location->getCgiPath().empty())
        {
            std::string boundary = "Content-Disposition: form-data; name=\"";
        std::string::size_type boundaryPos = client._requestStr.find(boundary);
    
        if (boundaryPos == std::string::npos)
        {
            sendHttpResponse(client, 400, "text/html", "Mauvaise requête");
            return;
        }
    
        std::string::size_type startPos = client._requestStr.find("filename=\"", boundaryPos);
    
        if (startPos == std::string::npos)
        {
            sendHttpResponse(client, 400, "text/html", "Mauvaise requête");
            return;
        }
    
        startPos += 10; // Passer "filename=\""
        std::string::size_type endPos = client._requestStr.find("\"", startPos);
        std::string filename = client._requestStr.substr(startPos, endPos - startPos);
    
        std::string contentTypeHeader = "Content-Type: ";
        startPos = client._requestStr.find(contentTypeHeader, endPos);
    
        if (startPos == std::string::npos)
        {
            sendHttpResponse(client, 400, "text/html", "Mauvaise requête");
            return;
        }
    
        startPos += contentTypeHeader.length();
        endPos = client._requestStr.find("\r\n", startPos);
        std::string contentType = client._requestStr.substr(startPos, endPos - startPos);
    
        // Trouver le début des données du fichier
        startPos = client._requestStr.find("\r\n\r\n", endPos);
        startPos += 4;
    
        // Trouver la fin des données du fichier
        endPos = client._requestStr.find("\r\n--", startPos);
    
        if (endPos == std::string::npos)
        {
            sendHttpResponse(client, 400, "text/html", "Mauvaise requête");
            return;
        }
    
        std::string fileData = client._requestStr.substr(startPos, endPos - startPos);
    
        // Écrire les données du fichier dans un fichier sur le serveur
        std::ofstream outFile("uploads/" + filename, std::ios::binary);
        outFile.write(fileData.c_str(), fileData.size());
        outFile.close();
    
        sendHttpResponse(client, 201, "text/html", "Fichier créé avec succès");
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

std::string Connection::getMimeType(const std::string& filePath)
{
   std::string fileExtension = filePath.substr(filePath.find_last_of('.'));
   if (_mimeTypes.find(fileExtension) != _mimeTypes.end())
       return _mimeTypes[fileExtension];
	else
       return "application/octet-stream"; // Type MIME par défaut ?? ou "text/plain"
}

std::string Connection::getFilePath(const Client &client)
{
    std::string basePath = client._config.getRoot();
    std::string filePath = basePath + client._uri;

    if (filePath.back() == '/')
        filePath += client._config.getIndex();

    return filePath;
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