/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 15:50:59 by estarck           #+#    #+#             */
/*   Updated: 2023/04/07 00:00:43 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Response.hpp"

void sendHttpResponse(Client &client, int statusCode, const std::string &contentType, const std::string &body)
{
    std::string response;
    std::string statusMessage;

    switch (statusCode)
    {
        case 200:
            statusMessage = "OK";
            break;
        case 201:
            statusMessage = "Created";
            break;
        case 204:
            statusMessage = "No Content";
            break;
        default:
            statusMessage = "Unknown Status";
    }

    response.append("HTTP/1.1 " + std::to_string(statusCode) + " " + statusMessage + "\r\n");
    response.append("Content-Type: " + contentType + "\r\n");
    response.append("Content-Length: " + std::to_string(body.length()) + "\r\n");
    response.append("Connection: Closed\r\n");
    response.append("\r\n");
    response.append(body);

    if (send(client._csock, response.c_str(), response.length(), 0) == -1)
    {
        perror("Erreur lors de l'envoi de la réponse");
    }
}

void sendErrorResponse(Client &client, int code)
{
    std::string response;
    std::string message;

    switch (code)
    {
        case 400:
            message = "Bad Request";
            break;
        case 403:
            message = "Forbidden";
            break;
        case 404:
            message = "Not Found";
            break;
        case 405:
            message = "Method Not Allowed";
            break;
		case 408:
			message = "Request Timeout";
			break;
		case 413:
			message = "Content Too Large";
			break;
		case 414:
			message = "URI Too Long";
			break;
        case 500:
            message = "Internal Server Error";
            break;
		case 505:
			message = "HTTP Version Not Supported";
			break;
        default:
            message = "Unknown Error";
    }

    response = "HTTP/1.1 " + std::to_string(code) + " " + message + "\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += "<!DOCTYPE html>\n";
    response += "<html lang=\"en\">\n";
    response += "<head>\n";
    response += "    <meta charset=\"UTF-8\">\n";
    response += "    <title>" + std::to_string(code) + " " + message + "</title>\n";
    response += "</head>\n";
    response += "<body>\n";
    response += "    <h1>Error " + std::to_string(code) + ": " + message + "</h1>\n";
    response += "</body>\n";
    response += "</html>";

    if (send(client._csock, response.c_str(), response.length(), 0) == -1)
        perror("Erreur lors de l'envoi de la réponse d'erreur");
}