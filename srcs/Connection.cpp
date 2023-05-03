/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 13:50:45 by estarck           #+#    #+#             */
/*   Updated: 2023/05/03 11:10:31 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Connection.hpp"
#include "./utils.hpp"


#include <stdexcept>
#include <iostream>
#include <algorithm>

Connection::Connection()
{}

Connection::Connection(std::vector<Server*>& servers) :
	_servers(servers),
	_maxFd(-1)
{
	_timeout.tv_sec = 3;
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
	_mimeTypes.insert(std::make_pair(".webp", "image/webp"));
	_mimeTypes.insert(std::make_pair(".png", "image/png"));
	_mimeTypes.insert(std::make_pair(".gif", "image/gif"));
	_mimeTypes.insert(std::make_pair(".mp4", "video/mp4"));
	_mimeTypes.insert(std::make_pair(".ico", "image/vnd.microsoft.icon"));
}

void Connection::initConnection()
{
	FD_ZERO(&_read);
	FD_ZERO(&_write);
	FD_ZERO(&_error);
	for (std::vector<Server *>::iterator it = _servers.begin(); it < _servers.end(); it++)
		initSelect((*it)->getSocket(), _read);
	for (std::vector<Client>::iterator it = _client.begin(); it < _client.end(); it++)
	{
		if (!((*it)._requestPars))
			initSelect(it->_csock, _read);
		else
			initSelect(it->_csock, _write);
		initSelect(it->_csock, _error);
	}
}

void Connection::initSelect(int fd, fd_set& set)
{
	FD_SET(fd, &set);
	if (fd > _maxFd)
		_maxFd = fd;
}

void Connection::runSelect()
{
	try
	{
		int res = select(_maxFd + 1, &_read, &_write, &_error, &_timeout);
		if (res < 0)
		{
			if (errno == EINTR) // Vérifier si l'erreur est due à un signal
				return;
			else
				throw std::runtime_error("select failed");
		}
		else if (res == 0 && (_timeout.tv_sec != 0 || _timeout.tv_usec != 0))
		{
			for (std::vector<Client>::iterator it = _client.begin(); it != _client.end(); it++)
				(*it)._keepAlive = false;
		}
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << "\033[0;31mErreur lors de l'appel à la fonction select : " << e.what() << "\033[0m" << std::endl;
		for (std::vector<Client>::iterator it = _client.begin(); it != _client.end(); it++)
			(*it)._keepAlive = false;
	}
}

void Connection::acceptSocket()
{
	for (std::vector<Server *>::iterator it = _servers.begin(); it < _servers.end(); it++)
	{
		if (FD_ISSET((*it)->getSocket(), &_read))
		{
			Client newClient((*it)->getConfig(), (*it)->getServer(), (*it)->getLocation());
			std::memset(&newClient._csin, 0, sizeof(newClient._csin));
			newClient._crecSize = sizeof(newClient._csin);
			if ((*it)->hasCapacity())
			{
				
				newClient._csock = accept((*it)->getSocket(), (sockaddr*)&newClient._csin, &newClient._crecSize);
				if (newClient._csock < 0)
				{
					std::cerr << "\033[31mFailed to accept connection \033[0mon port " << (*it)->getPort() << ", error code: " << errno << ", error message: " << strerror(errno) << std::endl;
					close(newClient._csock);
					continue;
				}
				if (fcntl(newClient._csock, F_SETFL, O_NONBLOCK) < 0)
					std::cerr << "\033[31mError : fcntl Connection::acceptSocket()\033[0m" <<std::endl;
				std::cerr << "\033[0;32m\nAccepted connection \033[0m on port " << (*it)->getPort() << std::endl;
				std::cerr << "\033[0;32mClient connected \033[0m on socket: " << newClient._csock << std::endl;

				(*it)->incrementCurrentConnection();
				_client.push_back(newClient);
			}
		}
	}
}

void Connection::clearClientSockets()
{
	for (std::vector<Client>::iterator it = _client.begin(); it < _client.end(); it++)
	{
		std::cout << "\033[32mLiberation des sockets clients\033[0m\n";
		if (close((*it)._csock) != 0)
			std::cerr << "\033[0;31mError while closing socket:\033[0m " << strerror(errno) << std::endl;
	}
	_client.clear();
}

// erase client si Connection=close
bool Connection::deadOrAlive(Client client, bool alive)
{
	if (alive)
		return false;
	client._server.decrementCurrentConnection();
	// Supprime le fd du socket read write error
	FD_CLR(client._csock, &_read);
	FD_CLR(client._csock, &_write);
	FD_CLR(client._csock, &_error);
	// ferme la connexion du socket client en spécifiant qu'il ne peut plus envoyer ni recevoir de données.
	shutdown(client._csock, SHUT_RDWR);
	if (close(client._csock) != 0)
		std::cerr << "\033[0;31mError while closing socket:\033[0m " << strerror(errno) << std::endl;

	std::cout << "\033[0;31mClient close \033[0m" << client._csock << std::endl;;
	return true;
}

void Connection::traitement()
{
	for (std::vector<Client>::iterator it = _client.begin(); it < _client.end(); it++)
	{
		if (FD_ISSET(it->_csock, &_error))
			(*it)._keepAlive = false;

		if ((*it)._keepAlive && FD_ISSET(it->_csock, &_read))
		{
			if (receiveClientRequest(*it))
			{
				(*it)._requestPars = true;
				FD_SET(it->_csock, &_write);
			}
		}

		if ((*it)._keepAlive && FD_ISSET(it->_csock, &_write))
		{
			if (handleReponse(*it))
				(*it)._keepAlive = false;
		}

		if (deadOrAlive((*it), (*it)._keepAlive))
			it = _client.erase(it);
	}
}

bool Connection::receiveClientRequest(Client &client)
{
	//On recupere la taille du tampon sur le socket.
	int optval = 0;
	socklen_t  optlen = sizeof(optval);
	if(getsockopt(client._csock, SOL_SOCKET, SO_RCVBUF, &optval, &optlen) == -1)
	{
		std::cerr << "\033[0;31mError : 500 receiving data from client getsockopt():\033[0m " << client._csock << std::endl;
		sendErrorResponse(client, 500);
		client._keepAlive = false;
		return false;
	}

	//Recupere le tampon avec une taille adaptee
	char buffer[optval];
	ssize_t bytesRead = recv(client._csock, buffer, optval, 0);
	if (bytesRead <= 0)
	{
		if (bytesRead == 0)
			std::cout << "Client disconnected on socket: " << client._csock << std::endl;
	  	else
		{
			std::cerr << "\033[0;31mError : 500 receiving data from client recv():\033[0m " << client._csock << std::endl;
			sendErrorResponse(client, 500);
		}
		client._keepAlive = false;
		return false;
	}

	if (bytesRead > optval)
	{
		std::cerr << "\033[0;31mError : 413 Request size exceeds the limit (" << optval << " bytes) for client:\033[0m " << client._csock << std::endl;
		sendErrorResponse(client, 413);
		return false;
	}

	//Clean avant ecriture sinon ca pose des problemes memoires d'overflow
	if (client._contentLenght == 0 && client._requestStr.str().empty() == 0)
	{
		client._requestStr.str(std::string());
		client._bodyReq.str(std::string());
	}

	client._requestStr.write(buffer, bytesRead);
	if (client._contentLenght == 0)
	{
		HTTPRequest httpRequest(client);
		if (client._method != POST)
		{
			std::memset(&buffer, 0, optval);
			return true;
		}
		if (client._uri == "/test_max")
		{
			ParsConfig::Location *location = findLocationForUri(client._uri, client._location);
			if (location->getMaxSize() < client._contentLenght)
			{
				sendErrorResponse(client, 413);
				client._keepAlive = false;
				return false;
			}
		}
	}
	else
	{
		client._bodyReq.write(buffer, bytesRead);
		client._sizeBody += bytesRead;
	}

	
	std::memset(&buffer, 0, optval);
   	if (client._sizeBody < client._contentLenght)
		return false;
   	return true;
}

bool Connection::handleReponse(Client &client)
{
	bool ret = true;
	switch (client._method)
	{
		case GET:
			ret = handleGET(client);
			break;
		case POST:
			handlePOST(client);
			break;
		case DELETE:
			handleDELETE(client);
			break;
		default:
			std::cerr << "Client " << client._csock << " connected with an unauthorized method." << std::endl;
			sendErrorResponse(client, 405);
			break;
	}
	return (ret);
}

bool Connection::handleGET(Client& client)
{
	if (hanglGetLocation(client))
		return (true);
	
	if (client._uri.length() >= MAX_URI_SIZE)
	{
		std::cerr << "\033[0;31mError : 414 URI Too Long from client:\033[0m " << client._csock << std::endl;
		sendErrorResponse(client, 414);
		return (true);
	}

	// On ouvre le fichier a renvoyer et on le stock dans client._bodyRep.
	// Pour eviter de le faire a chaque tour, on check si client._sizeBodyRep == 0
	if (client._sizeRep == 0)
	{
		client._filePath = getFilePath(client);
		std::ifstream	file(client._filePath, std::ios::in | std::ios::binary);
		if (file.is_open())
		{
			std::stringstream buf;
			buf << file.rdbuf();
			client._bodyRep = buf.str();
			client._sizeRep = 0;
			client._sizeSend = 0;
			file.close();
			buf.clear();
			createHttpResponse(client, 200, getMimeType(client._filePath));
		}
		else
		{
			std::cerr << "\033[0;31mError : 404 Not Found from client:\033[0m " << client._csock << std::endl;
			sendErrorResponse(client, 404);
		}
	}

	sendHttpResponse(client);
	if (client._sizeSend < client._sizeRep)
		return (false);
	return (true);
}

bool Connection::hanglGetLocation(Client &client)
{
	//Location and GET
	ParsConfig::Location *location = findLocationForUri(client._uri, client._location);
	if (location)
	{
		if (!(location->getCgiPath().empty()))
		{
			executeCGI(client, location);
			return (true);
		}

		
		if (location->getDeny())
		{
			std::cerr << "\033[0;31mError : 403 Forbidden:\033[0m " << client._csock << std::endl;
			sendErrorResponse(client, 403);
			return (true);
		}
		
		if (!(location->isMethodAllowed("GET")))
		{
			std::cerr << "\033[0;31mError : 405 Method Not Allowed from client:\033[0m " << client._csock << std::endl;
			sendErrorResponse(client, 405);
			return (true);
		}

		if (location->getAutoIndex())
		{
			std::cout << location->getPath();
			startAutoIndex(client, location->getPath());
			return (true);
		}
		std::string filePath = getFilePath(client, location);
		std::cout << "path : " << filePath << std::endl;
		std::ifstream file(filePath, std::ios::in | std::ios::binary);
		
		if (file.is_open() && location->getReturn().empty())
		{
			std::string line;
			while (getline(file, line))
    		{
				client._bodyRep.append(line + "\n");
    		    line.clear();
    		}
			createHttpResponse(client, 200, getMimeType(filePath));
			sendHttpResponse(client);
		}
		else if (!(location->getReturn().empty()))
		{
			client._response.append("HTTP/1.1 301 Moved Permanently\r\n");
			client._response.append("Location: " + location->getReturn() + "\r\n\r\n");
			client._sizeRep = client._response.size();
			sendHttpResponse(client);
		}
		else
		{
		std::cout << "Je suis la : " << location->getDeny() << std::endl;
			std::cerr << "\033[0;31mError : 404 Not Found from client:\033[0m " << client._csock << std::endl;
			sendErrorResponse(client, 404);
		}
		
		file.close();
		return (true);
	}
	return (false);
}

void Connection::handlePOST(Client& client)
{
	// Vérification si l'URI correspond à une configuration de location
	ParsConfig::Location *location = findLocationForUri(client._uri, client._location);
	if (!location || !location->isMethodAllowed("POST"))
	{
		std::cerr << "\033[0;31mError : 405 Method Not Allowed from client:\033[0m " << client._csock << std::endl;
		sendErrorResponse(client, 405);
		return;
	}
	if (location->getDeny())
	{
		std::cerr << "\033[0;31mError : 403 Forbidden:\033[0m " << client._csock << std::endl;
		sendErrorResponse(client, 403);
		return;
	}
	if (location->getUrl() == "/test_max")
	{
		createHttpResponse(client, 200, "text/html");
		sendHttpResponse(client);
		return;
	}
	
	// Si pas de cgiPath, on upload les données
	if (location->getCgiPath().empty())
	{
		//Recuperaton du Boundary
		std::string boundaryHeader = "Content-Type";
		std::string boundary = client._headers.find(boundaryHeader)->second.erase(0, std::strlen("multipart/form-data; boundary="));
		if (boundary.empty())
		{
			sendErrorResponse(client, 400);
			return;
		}

		//Recuperation du filename
		std::string 			filePartHeader = "filename=\"";
		std::string::size_type	filePartHeaderPos = client._requestStr.str().find(filePartHeader);
		if (filePartHeaderPos == std::string::npos)
		{
			sendErrorResponse(client, 400);
			return;
		}
		std::string::size_type startPos = filePartHeaderPos + filePartHeader.length();
		std::string::size_type endPos = client._requestStr.str().find("\"", startPos);
		std::string filename = client._requestStr.str().substr(startPos, endPos - startPos);
	
		//Check du content-type
		std::string contentTypeHeader = "Content-Type: ";
		startPos = client._requestStr.str().find(contentTypeHeader, endPos);
		if (startPos == std::string::npos)
		{
			sendErrorResponse(client, 400);
			return;
		}
		startPos += contentTypeHeader.length();
		endPos = client._requestStr.str().find("\r\n", startPos);
		std::string contentType = client._requestStr.str().substr(startPos, endPos - startPos);
		for (std::map<std::string, std::string>::iterator it = _mimeTypes.begin(); (*it).second == contentType; it++)
		{
			if (it == _mimeTypes.end())
			{
				sendErrorResponse(client, 400);
				return;
			}
		}

		// Trouver le début des données du fichier
		std::string body(client._bodyReq.str());
		startPos = body.find("\r\n\r\n");
		startPos += 4;

		// Trouver la fin des données du fichier
		boundary.replace(boundary.length(), 1, "--");
		endPos = body.find(boundary, startPos);
		endPos -= 4;
		if (endPos == std::string::npos)
		{
			sendErrorResponse(client, 400);
			return;
		}
		std::string fileData = body.substr(startPos, endPos - startPos);

		std::fstream outFile;
		outFile.open(location->getRoot() + "/" + filename, std::ios::binary | std::ios::out);
		if (outFile.is_open())
		{
			outFile << fileData;
			client._bodyRep = "Successfully created file";
			createHttpResponse(client, 201, "text/html");
			sendHttpResponse(client);
		}
		else 
		{
			sendErrorResponse(client, 400);
			return;
		}
		outFile.close();
	}
	else
	{
		// Lancer le cgiPath avec les données reçues
		executeCGI(client, location);
	}
}

void Connection::handleDELETE(Client& client)
{
	// Vérification si l'URI correspond à une configuration de location
	ParsConfig::Location *location = findLocationForUri(client._uri, client._location);
	if (!location || !location->isMethodAllowed("DELETE"))
	{
		std::cerr << "\033[0;31mError : 405 Method Not Allowed from client:\033[0m " << client._csock << std::endl;
		sendErrorResponse(client, 405);
		return;
	}

	//On recupere le nom du fichier dans la rquete
	std::istringstream	requestStream(client._requestStr.str());
	std::string			fileName;
	std::string			line;
	while (std::getline(requestStream, line))
	{
		size_t separator = line.find("\"file\":");
		if (separator != std::string::npos)
			fileName = line.substr(separator + 8, line.size() - 11);
	}
	
	//On supprime le fichier
	if (!(std::remove((location->getRoot() + "/" + fileName).c_str())))
	{
		client._bodyRep = fileName + " deleted";
		createHttpResponse(client, 200, "text/html");
		sendHttpResponse(client);
	}
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

std::string Connection::getFilePath(const Client &client,const ParsConfig::Location *location)
{
	std::string basePath = location->getRoot();
	std::string filePath = client._server.getRoot() + basePath + "/" + location->getIndex();
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

void Connection::executeCGI(Client &client, ParsConfig::Location *location)
{
	Cgi cgi(client, location);
	char **argv = cgi.arg(client);
	char **env = cgi.getenv();
	//char **argv = cgi.arg(client);

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
		signal(SIGPIPE, signal_handler);
		close(cgiInput[1]);
		close(cgiOutput[0]);

		dup2(cgiInput[0], STDIN_FILENO);
		dup2(cgiOutput[1], STDOUT_FILENO);

		if (execve(argv[0], argv, env) == -1)
		{
			sendErrorResponse(client, 500);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		close(cgiInput[0]);
		close(cgiOutput[1]);

		write(cgiInput[1], client._requestStr.str().c_str(), client._requestStr.str().length());
		close(cgiInput[1]);

		// Lire la sortie du script CGI depuis le pipe de sortie
		char buffer[4096];
		ssize_t bytesRead;
		while ((bytesRead = read(cgiOutput[0], buffer, sizeof(buffer) - 1)) > 0)
		{
			buffer[bytesRead] = '\0';
			client._bodyRep += buffer;
		}
		close(cgiOutput[0]);

		int status;
		waitpid(pid, &status, 0);

  
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		{
			createHttpResponse(client, 200, "text/html");
			sendHttpResponse(client);
		}
		else
			sendErrorResponse(client, 500);
	}
}