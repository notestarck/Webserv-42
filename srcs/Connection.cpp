/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 13:50:45 by estarck           #+#    #+#             */
/*   Updated: 2023/04/14 09:11:57 by estarck          ###   ########.fr       */
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
	_timeout.tv_sec = 2;
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
	FD_ZERO(&_error);
	for (std::vector<Server *>::iterator it = _servers.begin(); it < _servers.end(); it++)
		initSelect((*it)->getSocket(), _read);
	for (std::vector<Client>::iterator it = _client.begin(); it < _client.end(); it++)
	{
		initSelect(it->_csock, _read);
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
		std::cerr << "Erreur lors de l'appel à la fonction select : " << e.what() << std::endl;
		// Gérer l'exception ici, par exemple en fermant la connexion ou en renvoyant une erreur à l'utilisateur
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
					std::cerr << "Failed to accept connection on port " << (*it)->getPort() << ", error code: " << errno << ", error message: " << strerror(errno) << std::endl;
					close(newClient._csock);
					continue;
				}
				int flags = fcntl(newClient._csock, F_GETFL, 0);
				if (flags < 0)
					std::cout << "merde" << std::endl;
				if (fcntl(newClient._csock, F_SETFL, flags | O_NONBLOCK) < 0)
					std::cout << "zut" <<std::endl;
				std::cout << "\033[0;32m\nAccepted connection \033[0m on port " << (*it)->getPort() << std::endl;
				std::cout << "\033[0;32mClient connected \033[0m on socket: " << newClient._csock << std::endl;

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
	FD_CLR(client._csock, &_read);
	shutdown(client._csock, SHUT_RDWR);
	if (close(client._csock) != 0)
		std::cerr << "Error while closing socket: " << strerror(errno) << std::endl;

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
				FD_SET(it->_csock, &_write);
		}

		if ((*it)._keepAlive && FD_ISSET(it->_csock, &_write))
			handleReponse(*it);

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
		std::cerr << "Error : 500 receiving data from client getsockopt(): " << client._csock << std::endl;
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
			std::cerr << "Error : 500 receiving data from client recv(): " << client._csock << std::endl;
			sendErrorResponse(client, 500);
		}
		client._keepAlive = false;
		return false;
	}

	if (bytesRead > optval)
	{
		std::cerr << "Error : 413 Request size exceeds the limit (" << optval << " bytes) for client: " << client._csock << std::endl;
		sendErrorResponse(client, 413);
		return false;
	}

	//Clean avant ecriture sinon ca pose des problemes memoires d'overflow
	if (client._contentLenght == 0 && client._requestStr.str().empty() == 0)
	{
		client._requestStr.str(std::string());
		client._body.str(std::string());
	}

	client._requestStr << buffer;
	std::cout << client._requestStr.str() << std::endl;
	if (client._contentLenght == 0)
	{
		HTTPRequest httpRequest(client);
		if (client._method != POST)
		{
			std::memset(&buffer, 0, optval);
			return true;
		}
	}
	else
	{
		client._body.write(buffer, bytesRead);
		client._sizeBody += bytesRead;
	}
	
	std::memset(&buffer, 0, optval);
   	std::cout << "Taille contentLenght : " << client._contentLenght << " taille Body : " << client._sizeBody << std::endl;
   	if (client._sizeBody < client._contentLenght)
		return false;
   	return true;
}

bool Connection::handleReponse(Client &client)
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
			std::cerr << "Client " << client._csock << " connected with an unauthorized method." << std::endl;
			sendErrorResponse(client, 405);
			break;
	}
	client._keepAlive = false;
	return false;
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
		sendHttpResponse(client, 200, getMimeType(filePath), file);
		file.close();
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
		std::string body(client._body.str());
		startPos = body.find("\r\n\r\n");
		startPos += 4; 
		// Trouver la fin des données du fichier
		boundary.replace(boundary.length(), 1, "--");
		std::cout << "---------------- boundary : " << boundary << " et size body " << body.size() << std::endl;
		endPos = body.find(boundary, startPos);
		endPos -= 4;
		if (endPos == std::string::npos)
		{
			sendErrorResponse(client, 400);
			return;
		}
		std::string fileData = body.substr(startPos, endPos - startPos);

		std::fstream outfile;
		outfile.open(location->getRoot() + "/" + filename, std::ios::binary | std::ios::out);
		if (outfile.is_open())
		{
			outfile << fileData;
			sendHttpResponse(client, 201, "text/html", "Fichier créé avec succès");
		}
		else 
		{
			sendErrorResponse(client, 400);
			return;
		}
		outfile.close();
	}
	else
	{
		// Lancer le cgiPath avec les données reçues
		executeCGI(client, location->getCgiPath());
	}
}

void Connection::handleDELETE(Client& client)
{
	std::cout << "handleDELETE \n : " << client._requestStr.str() << std::endl;
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

		write(cgiInput[1], client._requestStr.str().c_str(), client._requestStr.str().length());
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