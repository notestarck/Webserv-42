/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 12:30:52 by estarck           #+#    #+#             */
/*   Updated: 2023/04/06 13:48:56 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest()
{}

HTTPRequest::HTTPRequest(const std::string &requestStr, Client &client) :
	_requestStr(requestStr)
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
	if (this != &srcs)
	{
		_requestStr = srcs._requestStr;
	}
	return (*this);
}

void HTTPRequest::parseRequest(Client &client)
{
    std::istringstream requestStream(_requestStr);
    std::string line;

    // Parse the request line
    if (std::getline(requestStream, line))
    {
        std::istringstream lineStream(line);
        std::string methodStr;
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
    while (std::getline(requestStream, line) && line != "\r")
    {
        size_t separator = line.find(": ");
        if (separator != std::string::npos)
        {
            std::string headerName = line.substr(0, separator);
            std::string headerValue = line.substr(separator + 2, line.size() - separator - 2 - 1); // -1 to exclude '\r'
            client._headers[headerName] = headerValue;
        }
    }
}