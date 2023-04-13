/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 12:30:52 by estarck           #+#    #+#             */
/*   Updated: 2023/04/13 17:48:35 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest()
{}

HTTPRequest::HTTPRequest(Client &client)
{
	parseRequest(client);
}

HTTPRequest::HTTPRequest(const HTTPRequest &srcs)
{
	*this = srcs;
}

HTTPRequest::~HTTPRequest()
{}

const HTTPRequest &HTTPRequest::operator=(const HTTPRequest &srcs)
{
	(void)srcs;
	return (*this);
}

void HTTPRequest::parseRequest(Client &client)
{
	std::istringstream	requestStream(client._requestStr.str());
	std::string			line;

	// Parse the request line
	if (std::getline(requestStream, line))
	{
		std::istringstream	lineStream(line);
		std::string			methodStr;
		lineStream >> methodStr >> client._uri >> client._httpVersion;

		if (methodStr == "GET")
			client._method = GET;
		else if (methodStr == "POST")
			client._method = POST;
		else if (methodStr == "DELETE")
			client._method = DELETE;
		else
			client._method = UNKNOWN;
	}

	// Parse headers
	while (std::getline(requestStream, line) && line.length() != 1)
	{
		size_t separator = line.find(": ");
		if (separator != std::string::npos)
		{
			std::string headerName = line.substr(0, separator);
			std::string headerValue = line.substr(separator + 2, line.size() - separator - 2 - 1);
			if (headerName == "Content-Length")
				client._contentLenght = std::atoi(headerValue.c_str());
			client._headers[headerName] = headerValue;
		}
	}
	if (client._method != POST)
	{
		requestStream.clear();
		line.clear();
		return;
	}
	
	 size_t posBody = client._requestStr.str().find("\r\n\r\n");
	 client._requestStr.seekg(posBody + 4);
	client._body.seekg(0, client._body.end);
	client._sizeBody = client._body.tellg();
	client._body.seekg(0, client._body.beg);
}