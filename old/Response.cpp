//
// Created by stephane walter on 19/03/2023.
//

#include "../include/Response.hpp"
#include <iostream>

Response::Response(std::string status) {
    //std::cout << "constuct Response\n";
    this->status_code = status.substr(0, 3);  // code status
    this->status_word = status.substr(4); // mot designant status


    //std::cout << status_code << std::endl;
    //std::cout << status_word << std::endl;
}

Response::~Response()
{}

void Response::append_header(std::string one, std::string two)
{
    headers.insert(std::make_pair(one, two));

}

int Response::get_body_size()
{
    return body.size();
}

void Response::set_body(std::string &str)
{
    this->body = str;
}

std::string Response::make_header()
{
    std::string result;

    result.append("HTTP/1.1 " + status_code + " " + status_word + "\r\n");
    for (std::map<std::string, std::string>::iterator i = headers.begin(); i != headers.end(); i++)
        result.append((*i).first + ": " + (*i).second + "\r\n");
    result.append("\r\n");

    return result;
}

void Response::make_status_body()
{
    std::string result;

    result.append("<!DOCTYPE html><html><head><meta charset=\"UTF-8\"/><title>webserv</title></head>");
    result.append("<body>");
    result.append("<h1>" + status_code + "</h1>");
    result.append("<h3>" + status_word + "</h3>");
    result.append("<p>Click <a href=\"/\">here</a> to return home.</p>");
    result.append("</body></html>");

    body.clear();
    body = result;
}

void Response::make_status_body(std::string url)
{
    std::string result;

    result.append(url);

    body.clear();
    body = result;
}

std::string Response::run_resp()
{
    std::string result;

    result.append("HTTP/1.1 " + status_code + " " + status_word + "\r\n");
    for (std::map<std::string, std::string>::iterator i = headers.begin(); i != headers.end(); i++)
        result.append((*i).first + ": " + (*i).second + "\r\n");
    result.append("\r\n");
    result.append(body);

    return result;
}